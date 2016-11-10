#include "eventLoop.h"
#include "epoller.h"
#include "channel.h"
#include "timerQueue.h"
#include "dbg.h"
#include "unistd.h"
#include <sys/eventfd.h>
#include <sys/signal.h>
using namespace cm;
const int kPollTimeMs = 10000;
__thread EventLoop* LoopInThisThread = NULL;

int createEventfd() {
	int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (fd < 0) {
		log_err("eventfd failed");
	}
	return fd;
}
class IgnoreSigPipe {
public:
	IgnoreSigPipe() {
		::signal(SIGPIPE, SIG_IGN);
	}
};
IgnoreSigPipe initObj;

EventLoop::EventLoop()
	: looping_(false)
	, quit_(false)
	, callingPendingFunctors_(false)
	, threadId_(CurrentThread::tid()) 
	, epoller_(new Epoller(this))
	, timerQueue_(new TimerQueue(this))
	, wakeupFd_(createEventfd())
	, wakeupChannel_(new Channel(this,wakeupFd_)){
		log_info("Eventloop create in thread %d", threadId_);
		if (LoopInThisThread) {
			log_err("Another Eventloop exists in this thread %d", CurrentThread::tid());
		}else {
			LoopInThisThread = this;	
		}
		wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
		wakeupChannel_->enableReading();
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
	 	epollReturnTime_ =  epoller_->poll(kPollTimeMs, &activeChannels_);
		for (auto&i : activeChannels_) {
			i->handleEvent(epollReturnTime_);
		}
		doPendingFunctors();
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


void cm::EventLoop::handleRead() {
	uint64_t one = 1;
	int n =::read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
		log_err("Eventloop handleRead error");
}


void cm::EventLoop::doPendingFunctors() {
	std::vector<Functor> functorTemp;
	callingPendingFunctors_ = true;
	{
		MutexGuard guard(mutex_);
		std::swap(pendingFunctors_, functorTemp);
	}
	for (auto &i : functorTemp)
		i();
	callingPendingFunctors_ = false;
}


void cm::EventLoop::wakeup() {
	uint64_t one = 1;
	int n =::write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
		log_err("Eventloop wakeup error");
}


void cm::EventLoop::queueInLoop(const Functor& functor) {
	{
		MutexGuard guard(mutex_);
		pendingFunctors_.push_back(functor);
	}
	if (!isInLoopThread() || callingPendingFunctors_) {
		wakeup();
	}
}

void EventLoop::quit() {
	quit_ = true;
	if (!isInLoopThread())
		wakeup();
}

void cm::EventLoop::removeChannel(Channel *channel) {
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	epoller_->removeChannel(channel);
}


void cm::EventLoop::runInLoop(const Functor&functor) {
	if (isInLoopThread())
		functor();
	else
		queueInLoop(functor);
}
