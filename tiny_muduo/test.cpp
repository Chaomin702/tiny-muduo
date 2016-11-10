#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
#include "eventLoopThread.h"
#include "socket.h"
#include "acceptor.h"
void newConnection(const cm::Socket& connSock, const cm::InetAddress& addr) {
	std::cout << "new connection accepted from " << addr.toHostPort() << std::endl;
	::write(connSock.fd(), "Hello~", 7);
}
void configure(cm::Acceptor *acceptor) {
	acceptor->setNewConnectionCallback(newConnection);
	acceptor->listen();
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::InetAddress listenAddr1(9981);
	cm::InetAddress listenAddr2(9982);
	cm::EventLoopThread loopThread1, loopThread2;
	
	cm::EventLoop *loop1 = loopThread1.startLoop();
	cm::EventLoop *loop2 = loopThread2.startLoop();
	cm::Acceptor acceptor1(loop1, listenAddr1);
	cm::Acceptor acceptor2(loop2, listenAddr2);
	loop1->queueInLoop(std::bind(&configure, &acceptor1));
	loop2->queueInLoop(std::bind(&configure, &acceptor2));	
	sleep(30);
	return 0;
}