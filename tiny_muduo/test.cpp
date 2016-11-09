#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
#include "eventLoopThread.h"
void print(const std::string& msg) {
	std::cout << msg << std::endl;
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::EventLoopThread et;
	cm::EventLoop *loop = et.startLoop();
	loop->queueInLoop(std::bind(&print, "anothread thread"));
	sleep(1);
	return 0;
}