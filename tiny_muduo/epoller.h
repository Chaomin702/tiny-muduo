#pragma once
#include "nonCopyable.h"
#include <vector>
#include <map>
#include "timeStamp.h"
struct epoll_event;

namespace cm 
{
	class EventLoop;
	class Channel;
	class Epoller :public NonCopyable {
	public:
		using ChannelList = std::vector<Channel*>;
		Epoller(EventLoop* loop);
		~Epoller();
		TimeStamp poll(int timeoutMs,ChannelList*);
		void updateChannel(Channel *channel);
	private:
		void update(int opt, Channel *channel);
		void fillUpdateChannels(int numEvents, ChannelList *activeChannels);
		using EventList = std::vector<struct epoll_event>;
		using ChannelMap = std::map<int, Channel*>;
		static const int KInitEventListSize = 16;
		EventLoop* ownerLoop_;
		int epollfd_;
		EventList events_;
		ChannelMap channels_;
	};
}