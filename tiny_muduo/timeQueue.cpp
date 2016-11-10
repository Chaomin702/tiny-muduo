#include "timerQueue.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include "dbg.h"
#include <functional>
#include <iterator>
#include "timer.h"
#include "eventLoop.h"
namespace cm {
	namespace detail {
		int createTimerfd() {
			int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
			if (timerfd < 0)
				log_err("timerfd create failed");
			return timerfd;
		}
		struct timespec howMuchTimeFromNow(TimeStamp when) {
			int64_t microseconds = when.microSecondsSinceEpoch() - TimeStamp::now().microSecondsSinceEpoch();
			if (microseconds < 100) //it's essential
				microseconds = 100;
			struct timespec ts;
			ts.tv_sec = static_cast<time_t>(microseconds / TimeStamp::kMicroSencondPerSecond);
			ts.tv_nsec = static_cast<long>((microseconds % TimeStamp::kMicroSencondPerSecond) * 1000);
			return ts;
		}
		void readTimerfd(int timerfd, TimeStamp now) {
			uint64_t howmany;
			ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
			log_info("TimerQueue readtimerfd %ld at %s", howmany, now.toString().c_str());
			if (n != sizeof howmany) {
				log_err("readTimerfd read error");
			}
		}
		void resetTimerfd(int timerfd, TimeStamp expiration) {
			struct itimerspec newValue;
			struct itimerspec oldValue;
			bzero(&newValue, sizeof newValue);
			bzero(&oldValue, sizeof oldValue);
			newValue.it_value = howMuchTimeFromNow(expiration);
			int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
			if (ret)
				log_err("timerfd reset failed %d", ret);
		}
	}
}

using namespace cm;
using namespace cm::detail;

TimerQueue::TimerQueue(EventLoop* loop)
	: loop_(loop)
	, timerfd_(createTimerfd())
	, timerfdChannel_(loop, timerfd_) {
	timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}
TimerQueue::~TimerQueue() {
	::close(timerfd_);
}
void TimerQueue::addTimer(const TimerCallback& cb, TimeStamp when, double interval) {
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, cb, when, interval));
}
void cm::TimerQueue::addTimerInLoop(const TimerCallback& cb, TimeStamp when, double interval) {
	loop_->assertInLoopThread();
	auto it = timers_.begin();
	bool earliestChanged = false;
	if (it == timers_.end() || when < it->first) {
		earliestChanged = true;
	}
	timers_.insert(std::make_pair(when, std::unique_ptr<Timer>(new Timer(cb, when, interval))));
	if (earliestChanged) {
		log_info("timer insert %ld", when.microSecondsSinceEpoch());
		resetTimerfd(timerfd_, when);
	}
}
void TimerQueue::handleRead() {
	loop_->assertInLoopThread();
	TimeStamp now = TimeStamp::now();
	readTimerfd(timerfd_, now);
	auto expireds = getExpired(now);
	for (auto&i : expireds)
		i->run();
	reset(expireds, now);
}
std::vector<std::unique_ptr<Timer>> TimerQueue::getExpired(TimeStamp now) {
	std::vector<std::unique_ptr<Timer>> expired;
	auto up = timers_.lower_bound(now);
	assert(up == timers_.end() || now < up->first);
	for (auto i = timers_.begin(); i != up; ++i) {
		expired.push_back(std::move(i->second));
	}
	timers_.erase(timers_.begin(), up);
	return expired;
}

void TimerQueue::reset(std::vector<std::unique_ptr<Timer>>&expired, TimeStamp now) {
	for (auto&i : expired) {
		if (i->repeat()) {
			i->restart(now);
			timers_.insert(std::make_pair(i->expiration(), std::move(i)));
		}
	}
	TimeStamp nextExpire;
	if (!timers_.empty()) {
		nextExpire = timers_.begin()->second->expiration();
	}
	if (nextExpire.valid()) {
		resetTimerfd(timerfd_, nextExpire);
	}
}