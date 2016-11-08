#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
cm::EventLoop* g_loop;
void timeout()
{
  printf("Timeout!\n");
  g_loop->quit();
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::EventLoop loop;
	g_loop = &loop;
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	cm::Channel channel(&loop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();
	
	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	::timerfd_settime(timerfd, 0, &howlong, NULL);
	
	loop.loop();
	::close(timerfd);
	return 0;
}