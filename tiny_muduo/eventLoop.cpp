#include "eventLoop.h"
#include "dbg.h"
using namespace cm;

__thread EventLoop* LoopInThisThread = NULL;
EventLoop::EventLoop()
	: looping_(false)
	, threadId_(CurrentThread::tid()) {
		log_info("Eventloop create in thread %d", threadId_);
		if (LoopInThisThread) {
			log_err("Another Eventloop exists in this thread %d", CurrentThread::tid());
		}else {
			LoopInThisThread = this;	
		}
	}
EventLoop::~EventLoop() {
	LoopInThisThread = NULL;
}
void EventLoop::abortNotInLoopThread() {
	log_err("Eventloop created in thread %d, but current thread is %d", threadId_, CurrentThread::tid());
}

void EventLoop::loop() {
	assertInLoopThread();
	log_info("EventLoop stop running");
}

