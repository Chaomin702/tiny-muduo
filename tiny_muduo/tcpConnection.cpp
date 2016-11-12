#include "tcpConnection.h"
#include "eventLoop.h"
#include "dbg.h"
#include <unistd.h>
using namespace cm;
using namespace std::placeholders;

cm::TcpConnection::TcpConnection(EventLoop * loop, const std::string name, const Socket& sockfd, const InetAddress & localAddr, const InetAddress & peerAddr) 
	: loop_(loop)
	, name_(name)
	, state_(kConnecting)
	, socket_(new Socket(std::move(const_cast<Socket&>(sockfd))))
	, channel_(new Channel(loop_,socket_->fd()))
	, localAddr_(localAddr)
	, peerAddr_(peerAddr) {
		channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
}

cm::TcpConnection::~TcpConnection() {
	log_info("TcpConnection %s at %s closed", name_.c_str(), peerAddr_.toHostPort().c_str());
}

void cm::TcpConnection::connectionEstablished() {
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	state_ = kConnected;
	channel_->enableReading();
	connectionCallback_(shared_from_this());
}
void cm::TcpConnection::connectionDestroyed() {
	loop_->assertInLoopThread();
	assert(state_ == kDisconnecting || state_ == kConnected);
	state_ = kDisconnected;
	channel_->disableAll();
	connectionCallback_(shared_from_this());
	loop_->removeChannel(channel_.get());
}
void cm::TcpConnection::handleRead(TimeStamp receiveTime) {
	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(socket_->fd(), &savedErrno);
	if (n > 0)
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	else if (n == 0)
		handleClose();
	else {
		errno = savedErrno;
		log_warn("TCP connection handle read error");
		handleError();
	 }
}


void cm::TcpConnection::handleWrite() {
	loop_->assertInLoopThread();
	if (channel_->isWriting()) {
		ssize_t n = write(socket_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
		if (n > 0) {
			outputBuffer_.retrieve(n);
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();
				if (state_ == TcpConnection::kDisconnecting)
					shutdownInLoop();
			}
			else {
				log_info("I am going to write more data");
			}
		}
		else {
			log_err("Tcp handle write error");
		}
	}else{
		log_info("connection is down, no more writing");
	}
}

void cm::TcpConnection::handleClose() {
	loop_->assertInLoopThread();
	assert(state_ == kConnected || state_ == TcpConnection::kDisconnecting);
	channel_->disableAll();
	closeCallback_(shared_from_this());
}


void cm::TcpConnection::handleError() {
	log_warn("TCP connection handle error");
}


void cm::TcpConnection::sendInLoop(const std::string& message) {
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote =::write(socket_->fd(), message.data(), message.size());
		if (nwrote > 0) {
			if (nwrote < message.size())
				log_info("I am going to write more data");
		}else{
			nwrote = 0;
			if (errno != EWOULDBLOCK)
				log_err("TCP connection sendInloop error");
		}
	}
	assert(nwrote >= 0);
	if (nwrote < message.size()) {
		outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
		if (!channel_->isWriting()) {
			channel_->enableWriting();
		}
	}
}


void cm::TcpConnection::shutdownInLoop() {
	loop_->assertInLoopThread();
	if (!channel_->isWriting())
		socket_->shutdownWrite();
}


void cm::TcpConnection::shutdown() {
	if (state_ == kConnected) {
		state_ = kDisconnecting;
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this)); //bind持有TcpConnection的this指针，但只有在close时指针才会失效吧。。
	}
}


void cm::TcpConnection::send(const std::string& message) {
	if (state_ == TcpConnection::kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message);
		}else{
			loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
		}
	}
}
