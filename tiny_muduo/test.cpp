#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
#include "socket.h"
#include "acceptor.h"
#include "tcpServer.h"
#include "tcpConnection.h"
void newConnection(const cm::Socket& connSock, const cm::InetAddress& addr) {
	std::cout << "new connection accepted from " << addr.toHostPort() << std::endl;
	::write(connSock.fd(), "Hello~", 7);
}
void configure(cm::Acceptor *acceptor) {
	acceptor->setNewConnectionCallback(newConnection);
	acceptor->listen();
}
void onConnection(const cm::TcpConnetionPtr& conn) {
	if (conn->connected()) {
		std::cout << "new connection " << conn->name() << " from " << conn->peerAddress().toHostPort() << std::endl;
	}else{
		std::cout << "connection " << conn->name() << " from " << conn->peerAddress().toHostPort()<< " is close" << std::endl;
	}
}
void onMessage(const cm::TcpConnetionPtr& conn, const char* data, ssize_t len) {
	std::cout << "onMessage(): received " << len << "bytes from connection" << conn->name().c_str() << std::endl;
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::InetAddress listenAddr(3000);
	cm::EventLoop loop;
	cm::TcpServer server(&loop, listenAddr);
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.start();
	loop.loop();
//	cm::Acceptor acceptor(&loop, listenAddr);
//	acceptor.setNewConnectionCallback(newConnection);
//	acceptor.listen();
//	loop.loop();
	return 0;
}