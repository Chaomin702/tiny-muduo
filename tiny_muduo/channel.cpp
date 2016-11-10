#include "channel.h"
#include "sys/epoll.h"
#include "eventLoop.h"
#include "dbg.h"
using namespace cm;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
	: loop_(loop)
	, fd_(fd)	
	, events_(0)
	, revents_(0)
	, index_(-1)
	, eventHandling(false){}
Channel::~Channel() {
	assert(!eventHandling);
}
void Channel::update() {
	loop_->updateChannel(this);
}

void Channel::handleEvent(TimeStamp receiveTime) {
	eventHandling = true;
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
		log_info("hangle event close!");
		if (closeCallback_) closeCallback_();
	}
	if(revents_&EPOLLERR){
		if (errorCallback_)errorCallback_();
	}
	if (revents_&(EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
		if (readCallback_) readCallback_(receiveTime);
	}
	if (revents_&EPOLLOUT) {
		if (writeCallback_) writeCallback_();
	}
	eventHandling = false;
}
