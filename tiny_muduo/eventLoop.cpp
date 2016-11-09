#include "eventLoop.h"
#include "epoller.h"
#include "channel.h"
#include "timerQueue.h"
#include "dbg.h"
using namespace cm;
const int kPollTimeMs = 1000;
__thread EventLoop* LoopInThisThread = NULL;
EventLoop::EventLoop()
	: looping_(false)
	, quit_(false)
	, threadId_(CurrentThread::tid()) 
	, epoller_(new Epoller(this))
	, timerQueue_(new TimerQueue(this)){
		log_info("Eventloop create in thread %d", threadId_);
		if (LoopInThisThread) {
			log_err("Another Eventloop exists in this thread %d", CurrentThread::tid());
		}else {
			LoopInThisThread = this;	
		}
	}
EventLoop::~EventLoop() {
	assert(!looping_);
	LoopInThisThread = NULL;
}
void EventLoop::abortNotInLoopThread() {
	log_err("Eventloop created in thread %d, but current thread is %d", threadId_, CurrentThread::tid());
}

void EventLoop::loop() {
	looping_ = true;
	assertInLoopThread();
	log_info("EventLoop start running");
	while (!quit_) {
		activeChannels_.clear();
		epoller_->poll(kPollTimeMs, &activeChannels_);
		for (auto&i : activeChannels_) {
			i->handleEvent();
		}
	}
	looping_ = false;
	log_info("EventLoop stop running");
}

void EventLoop::updateChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	epoller_->updateChannel(channel);
}


void cm::EventLoop::runAt(const TimeStamp& time, const TimerCallback& cb) {
	timerQueue_->addTimer(cb, time, 0);
}


void cm::EventLoop::runAfter(double delay, const TimerCallback& cb) {
	TimeStamp time(addTime(TimeStamp::now(), delay));
	runAt(time, cb);
}




void cm::EventLoop::runEvery(double interval, const TimerCallback& cb) {
	TimeStamp time(addTime(TimeStamp::now(), interval));
	timerQueue_->addTimer(cb, time, interval);
}
