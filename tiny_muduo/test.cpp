#include <iostream>
#include <unistd.h>
#include "eventLoop.h"

cm::EventLoop loop;
void func() {
	loop.loop();
}
int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << cm::CurrentThread::tid() << "\n";
	cm::Thread t(func);
	t.start();
	loop.loop();
	t.join();
	return 0;
}