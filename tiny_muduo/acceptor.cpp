#include "acceptor.h"
#include "socketOps.h"
#include "eventLoop.h"
using namespace cm;



cm::Acceptor::Acceptor(EventLoop *loop, const InetAddress&addr)
	: loop_(loop)
	, listenning_(false)
	, acceptSocket_(sockets::createNonblocking())
	, acceptChannel_(loop_, acceptSocket_.fd()) {
		acceptSocket_.setReuseAddr(true);
		acceptSocket_.bindAddress(addr);
		acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
	}


void cm::Acceptor::listen() {
	std::cout << "test" << std::endl;
	loop_->assertInLoopThread();
	std::cout << "test over" << std::endl;
	listenning_ = true;
	acceptChannel_.enableReading();
	acceptSocket_.listen();
}


void cm::Acceptor::handleRead() {
	loop_->assertInLoopThread();
	InetAddress addr(0);
	Socket connSocket(acceptSocket_.accept(&addr));
	if (connSocket.fd() > 0) {
		if (newConnectionCallback_)
			newConnectionCallback_(std::move(connSocket), addr);
	}
}
