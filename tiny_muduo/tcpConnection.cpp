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
}


void cm::TcpConnection::handleClose() {
	loop_->assertInLoopThread();
	log_info("TCP connection close handle");
	assert(state_ = kConnected);
	//channel_->disableAll();
	closeCallback_(shared_from_this());
}


void cm::TcpConnection::handleError() {
	log_warn("TCP connection handle error");
}
