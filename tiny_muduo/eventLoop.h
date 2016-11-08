#pragma once
#include <vector>
#include <memory>
#include "nonCopyable.h"
#include "thread.h"
namespace cm 
{
	class Channel;
	class Epoller;
	class EventLoop : public NonCopyable {
	public:
		EventLoop();
		~EventLoop();
		void loop();
		void quit() {quit_ = true;}
		void assertInLoopThread() {
			if (!isInLoopThread()) {
				abortNotInLoopThread();
			}
		}
		bool isInLoopThread(){return threadId_ == CurrentThread::tid();}
		void updateChannel(Channel *channel);
	private :
		using ChannelList = std::vector<Channel*>;
		void abortNotInLoopThread();
		bool looping_;
		bool quit_;
		const pid_t threadId_;
		std::unique_ptr<Epoller> epoller_;
		ChannelList activeChannels_;
	};
	
}