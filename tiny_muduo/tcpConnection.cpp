#include "tcpConnection.h"
#include "eventLoop.h"
#include "dbg.h"
#include <unistd.h>
using namespace cm;


cm::TcpConnection::TcpConnection(EventLoop * loop, const std::string name, const Socket& sockfd, const InetAddress & localAddr, const InetAddress & peerAddr) 
	: loop_(loop)
	, name_(name)
	, state_(kConnecting)
	, socket_(new Socket(std::move(const_cast<Socket&>(sockfd))))
	, channel_(new Channel(loop_,socket_->fd()))
	, localAddr_(localAddr)
	, peerAddr_(peerAddr) {
		channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
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
void cm::TcpConnection::handleRead() {
	char buf[65536];
	ssize_t n = read(socket_->fd(), buf, sizeof buf);
	if (n > 0)
		messageCallback_(shared_from_this(), buf, n);
	else if (n == 0)
		handleClose();
	else
		handleError();
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
