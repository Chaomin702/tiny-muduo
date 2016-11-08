#pragma once
#include "nonCopyable.h"
#include "thread.h"
namespace cm 
{
	class EventLoop : public NonCopyable {
	public:
		EventLoop();
		~EventLoop();
		void loop();
		void assertInLoopThread() {
			if (!isInLoopThread()) {
				abortNotInLoopThread();
			}
		}
		bool isInLoopThread(){return threadId_ == CurrentThread::tid();}
	private :
		void abortNotInLoopThread();
		bool looping_;
		const pid_t threadId_;
	};
	
}