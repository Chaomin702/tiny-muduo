#pragma once
#include "nonCopyable.h"
#include <functional>
namespace cm 
{
	class EventLoop;
	
	class Channel : public NonCopyable {
	public:
		using EventCallback = std::function<void()>;
		Channel(EventLoop *loop, int fd);
		//~Channel();
		void handleEvent();
		void setReadCallback(const EventCallback& callback) {readCallback_ = callback;}
		void setWriteCallback(const EventCallback& callback) {writeCallback_ = callback;}
		void setErrorCallback(const EventCallback& callback) {errorCallback_ = callback;}
		void setRevents(int revents) {revents_ = revents;}
		void setIndex(int index) {index_ = index;}
		int fd()const {return fd_;}
		int events()const {return events_;}
		int index()const {return index_;}
		void enableReading() {events_ |= kReadEvent; update();}
		EventLoop* ownerLoop() {return loop_;}
	private:
		void update();
		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;
		EventLoop *loop_;
		const int fd_;
		int events_;
		int revents_;
		int index_;
		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback errorCallback_;
		
	};
}