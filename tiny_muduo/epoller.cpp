#include "epoller.h"
#include "dbg.h"
#include "channel.h"
#include "sys/epoll.h"
#include "errno.h"
#include "unistd.h"
#include "eventLoop.h"
using namespace cm;
const int KNew = -1;
const int KAdded = 1;
const int KDelete = 2;
Epoller::Epoller(EventLoop *loop)
	: ownerLoop_(loop)
	, epollfd_(::epoll_create1(EPOLL_CLOEXEC))
	, events_(KInitEventListSize) {
		if (epollfd_ < 0)
			log_err("Epoller init failed");
	}

Epoller::~Epoller() {
	::close(epollfd_);
}

void Epoller::update(int opt, Channel *channel) {
	struct epoll_event event;
	bzero(&event, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();
	if (::epoll_ctl(epollfd_ , opt, fd, &event) < 0) {
		log_err("epoller update operation failed");
	}
}

TimeStamp Epoller::poll(int timeoutMs, ChannelList* activaChannelList) {
	int numEvents =::epoll_wait(epollfd_, events_.data(), events_.size(), timeoutMs);
	TimeStamp t = TimeStamp::now();
	if (numEvents > 0) {
		fillUpdateChannels(numEvents, activaChannelList);
	}else if (numEvents == 0) {
		log_info("epoller: nothing happended");
	}else{
		log_err("epoller: wait error");
	}
	return t;
}

void Epoller::fillUpdateChannels(int numEvents, ChannelList *activeChannels) {
	assert(numEvents < events_.size());
	for (int i = 0; i < numEvents; ++i) {
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		int fd = channel->fd();
		assert(channels_[fd] == channel);
		channel->setRevents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void Epoller::updateChannel(Channel *channel) {
	ownerLoop_->assertInLoopThread();
	const int index = channel->index();
	int fd = channel->fd();
	if (index == KNew || index == KDelete) { //Ôø¾­É¾³ý¹ýµÄ
		if (index == KNew) {
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		 }else{
			 assert(channels_.find(fd) != channels_.end());
			 assert(channels_[fd] == channel);
		 }
		update(EPOLL_CTL_ADD, channel);
		channel->setIndex(KAdded);
	}else{
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == KAdded);
		if (channel->isNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->setIndex(KDelete);
		}else{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void cm::Epoller::removeChannel(Channel *channel) {
	ownerLoop_->assertInLoopThread();
	int fd = channel->fd();
	log_info("fd = %d", fd);
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());
	int index = channel->index();
	assert(index == KAdded || index == KDelete);
	channels_.erase(fd);
	if (index == KAdded)
		update(EPOLL_CTL_DEL, channel);
	channel->setIndex(KNew);
}
