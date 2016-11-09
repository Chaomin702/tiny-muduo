#pragma once
#include <map>
#include <memory>
#include <vector>
#include "timeStamp.h"
#include "callback.h"
#include "channel.h"
#include "mutexLock.h"
namespace cm 
{
	class EventLoop;
	class Timer;
	class TimerQueue :public NonCopyable {
	public:
		TimerQueue(EventLoop* loop);
		~TimerQueue();
		void addTimer(const TimerCallback& cb, TimeStamp when, double interval);
	private:
		using TimerList = std::multimap<TimeStamp, std::unique_ptr<Timer>>;
		void addTimerInLoop(const TimerCallback& cb, TimeStamp when, double interval);
		void handleRead();
		std::vector<std::unique_ptr<Timer>> getExpired(TimeStamp now);
		void reset(std::vector<std::unique_ptr<Timer>>&expired, TimeStamp now);
		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;
		TimerList timers_;
	};
}

