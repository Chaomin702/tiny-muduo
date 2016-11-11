#include "tcpServer.h"
#include "eventLoop.h"
#include "assert.h"
#include "dbg.h"
#include "socketOps.h"
#include "tcpConnection.h"
using namespace cm;
using namespace std::placeholders;


cm::TcpServer::TcpServer(EventLoop *loop, 
	const InetAddress& listenAddr)
	: loop_(loop)
	, name_(listenAddr.toHostPort())
	, started_(false)
	, nextConnId_(1)
	, acceptor_(new Acceptor(loop, listenAddr)) 
	, threadPoll_(new EventLoopThreadPoll(loop)){
		acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
	}



cm::TcpServer::~TcpServer() {
}


void cm::TcpServer::start() {
	assert(!started_);
	started_ = true;
	threadPoll_->start();
	loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
}


void cm::TcpServer::newConnection(const Socket& connSocket, const InetAddress& peerAddr) {
	loop_->assertInLoopThread();
	char buf[32];
	snprintf(buf, sizeof buf, "#%d", nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;
	log_info("new connection socket fd=%d", connSocket.fd());
	InetAddress localAddr(sockets::getLocalAddr(connSocket.fd()));
	EventLoop *ioLoop = threadPoll_->getnextLoop();
	TcpConnetionPtr conn(new TcpConnection(ioLoop, connName, connSocket, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,_1));
	ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished,conn.get()));
}


void cm::TcpServer::removeConnection(const TcpConnetionPtr& conn) {
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}


void cm::TcpServer::removeConnectionInLoop(const TcpConnetionPtr& conn) {
	loop_->assertInLoopThread();
//	log_info("TCP server remove connection in loop [%s]", conn->name().c_str());
	for (auto &i : connections_)
		std::cout << i.first << std::endl;
	assert(connections_.find(conn->name()) != connections_.end());
	connections_.erase(conn->name());
	EventLoop *ioLoop = conn->getloop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}
