#pragma once
#include "nonCopyable.h"
#include "thread.h"
#include "mutexLock.h"
#include "condition.h"
namespace cm 
{
	class EventLoop;
	class EventLoopThread : public NonCopyable {
	public:
		EventLoopThread();
		~EventLoopThread();
		EventLoop* startLoop();
		
	private:
		void threadFunc();
		bool exiting_;
		EventLoop *loop_;
		MutexLock mutex_;
		Condition cond_;
		Thread thread_;
	};
}