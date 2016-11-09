#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
cm::EventLoop* g_loop;
void print(const std::string& msg) {
	std::cout << msg << std::endl;
}
void func() {
	g_loop->runAfter(1.0, std::bind(&print, "should be success"));
}
void run() {
	g_loop->queueInLoop(func);
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::EventLoop loop;
	g_loop = &loop;
	cm::Thread t(run);
	loop.runAfter(2.0, std::bind(&print, "2s delay"));
	t.start();
	loop.loop();

	return 0;
}