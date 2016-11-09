#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
cm::EventLoop* g_loop;
void print(const std::string& msg) {
	std::cout << "pid=" << ::getpid() << " tid=" << cm::CurrentThread::tid() << std::endl;
	std::cout << "now " << cm::TimeStamp::now().toString() << std::endl;
	std::cout << "msg: " << msg << std::endl;
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::EventLoop loop;
	g_loop = &loop;
	
	loop.runAfter(2, std::bind(&print, "2 second"));
	loop.runAfter(1, std::bind(&print, "1 second"));
	loop.runAfter(1.5, std::bind(&print, "1.5 second"));
	loop.runEvery(1, std::bind(&print, "1 every"));
	
	
	loop.loop();

	return 0;
}