#pragma once
#include <vector>
#include <memory>
#include "nonCopyable.h"
#include "thread.h"
#include "callback.h"
#include "timeStamp.h"
#include "mutexLock.h"
namespace cm 
{
	class Channel;
	class Epoller;
	class TimerQueue;
	class EventLoop : public NonCopyable {
	public:
		using Functor = std::function<void()>;
		EventLoop();
		~EventLoop();
		void loop();
		void quit();
		void assertInLoopThread() {
			if (!isInLoopThread()) {
				abortNotInLoopThread();
			}
		}
		TimeStamp epollReturnTime()const {return epollReturnTime_;}
		void runAt(const TimeStamp& time, const TimerCallback& cb);
		void runAfter(double delay, const TimerCallback& cb);
		void runEvery(double interval, const TimerCallback& cb);
		bool isInLoopThread(){return threadId_ == CurrentThread::tid();}
		void updateChannel(Channel *channel);
		void removeChannel(Channel *channel);
		void wakeup();
		void queueInLoop(const Functor&);
		void runInLoop(const Functor&);
	private :
		using ChannelList = std::vector<Channel*>;
		
		void abortNotInLoopThread();
		void handleRead();
		void doPendingFunctors();
		
		
		bool looping_;
		bool quit_;
		bool callingPendingFunctors_;
		TimeStamp epollReturnTime_;
		const pid_t threadId_;
		std::unique_ptr<Epoller> epoller_;
		std::unique_ptr<TimerQueue> timerQueue_;
		int wakeupFd_;
		std::unique_ptr<Channel> wakeupChannel_;
		MutexLock mutex_;
		ChannelList activeChannels_;
		std::vector<Functor> pendingFunctors_;
	};
	
}