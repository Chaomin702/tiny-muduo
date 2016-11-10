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
	, acceptor_(new Acceptor(loop, listenAddr)) {
		acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
	}



cm::TcpServer::~TcpServer() {
}


void cm::TcpServer::start() {
	assert(!started_);
	started_ = true;
	loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
}


void cm::TcpServer::newConnection(const Socket& connSocket, const InetAddress& peerAddr) {
	loop_->assertInLoopThread();
	std::cout << "test new connection comming~" << std::endl;
	char buf[32];
	snprintf(buf, sizeof buf, "#%d", nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;
	log_info("new TCP connection [%s] from %s", connName.c_str(), peerAddr.toHostPort().c_str());
	InetAddress localAddr(sockets::getLocalAddr(connSocket.fd()));
	TcpConnetionPtr conn(new TcpConnection(loop_, connName, connSocket, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,_1));
	conn->connectionEstablished();
}


void cm::TcpServer::removeConnection(const TcpConnetionPtr& conn) {
	loop_->assertInLoopThread();
	log_info("TCP server remove connection [%s]", conn->name().c_str());
	for (auto &i : connections_)
		std::cout << i.first << std::endl;
	assert(connections_.find(conn->name()) != connections_.end());
	connections_.erase(conn->name());
	loop_->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}
