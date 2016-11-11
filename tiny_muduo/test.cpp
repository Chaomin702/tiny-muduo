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

std::string message1, message2;
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
		std::cout << "new connection " << conn->name() << 
			" from " << conn->peerAddress().toHostPort() 
		<< " thread id=" << cm::CurrentThread::tid() << std::endl;
		conn->send(message1);
		conn->send(message2);
		conn->shutdown();
	}else{
		std::cout << "connection " << conn->name() << " from " << conn->peerAddress().toHostPort()<< " is close" << std::endl;
	}
}
void onMessage(const cm::TcpConnetionPtr& conn, cm::Buffer *buf, cm::TimeStamp receiveTime ) {
	std::cout << "onMessage(): received " << buf->readableBytes() 
		<< "bytes from connection" << conn->name().c_str() 
		<< " at " << receiveTime.toFormattedString().c_str() << std::endl;
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	int len = 10;
	message1.resize(len);
	message2.resize(len);
	std::fill(message1.begin(), message1.end(), 'A');
	std::fill(message2.begin(), message2.end(), 'B');
	cm::InetAddress listenAddr(3000);
	cm::EventLoop loop;
	cm::TcpServer server(&loop, listenAddr);
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.setThreadNum(5);
	server.start();
	loop.loop();
	return 0;
}