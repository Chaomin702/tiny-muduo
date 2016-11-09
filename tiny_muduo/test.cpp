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
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::InetAddress listenAddr(9981);
	cm::EventLoop loop;
	cm::Acceptor acceptor(&loop, listenAddr);
	acceptor.setNewConnectionCallback(newConnection);
	acceptor.listen();
	loop.loop();
	return 0;
}