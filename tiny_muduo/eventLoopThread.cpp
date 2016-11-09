#include "eventLoopThread.h"
#include <functional>
#include "eventLoop.h"

cm::EventLoopThread::EventLoopThread()
	: exiting_(false)
	, loop_(NULL)
	, cond_(mutex_)
	, thread_(std::bind(&EventLoopThread::threadFunc,this)) {}



cm::EventLoop* cm::EventLoopThread::startLoop() {
	assert(!exiting_);
	thread_.start();
	{
		MutexGuard lock(mutex_);
		while (loop_ == NULL) {
			cond_.wait();
		}
	}
	return loop_;
}


void cm::EventLoopThread::threadFunc() {
	EventLoop loop;
	{
		MutexGuard lock(mutex_);
		loop_ = &loop;
		cond_.notify();
	}
	loop.loop();
}


cm::EventLoopThread::~EventLoopThread() {
	exiting_ = true;
	loop_->quit();
	thread_.join();
}
