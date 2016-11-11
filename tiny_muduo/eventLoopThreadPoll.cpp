#include "eventLoop.h"
#include "eventLoopThread.h"
#include "eventLoopThreadPoll.h"

using namespace cm;



cm::EventLoopThreadPoll::EventLoopThreadPoll(EventLoop *baseloop)
	: started_(false)
	, threadNum_(0)
	, baseLoop_(baseloop)
	, next_(0) {}



void cm::EventLoopThreadPoll::start() {
	assert(!started_);
	baseLoop_->assertInLoopThread();
	started_ = true;
	for (int i = 0; i < threadNum_; ++i) {
		std::unique_ptr<EventLoopThread> t(new EventLoopThread());
		loops_.push_back(t->startLoop());
		thread_.push_back(std::move(t));
	}
}


cm::EventLoop* cm::EventLoopThreadPoll::getnextLoop() {
	baseLoop_->assertInLoopThread();
	EventLoop *loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[next_];
		next_++;
		if (next_ >= loops_.size())
			next_ = 0;
	}
	return loop;
}


cm::EventLoopThreadPoll::~EventLoopThreadPoll() {
}
