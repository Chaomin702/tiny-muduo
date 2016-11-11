#pragma once
#include <vector>
#include <memory>
#include "nonCopyable.h"
namespace cm 
{
	class EventLoop;
	class EventLoopThread;
	class EventLoopThreadPoll : public NonCopyable {
	public:
		EventLoopThreadPoll(EventLoop *baseloop);
		~EventLoopThreadPoll();
		void setThreadNum(int threadNum) {threadNum_ = threadNum;}
		void start();
		EventLoop* getnextLoop();
	private:
		bool started_;
		int threadNum_;
		EventLoop *baseLoop_;
		int next_;
		std::vector<std::unique_ptr<EventLoopThread>> thread_;
		std::vector<EventLoop*> loops_;
	};
}