#pragma once
#include "nonCopyable.h"
#include <functional>
#include "timeStamp.h"
namespace cm 
{
	class EventLoop;
	
	class Channel : public NonCopyable {
	public:
		using EventCallback = std::function<void()>;
		using readEventCallback = std::function<void(TimeStamp)>;
		Channel(EventLoop *loop, int fd);
		~Channel();
		void handleEvent(TimeStamp receiveTime);
		void setReadCallback(const readEventCallback& callback) {readCallback_ = callback;}
		void setWriteCallback(const EventCallback& callback) {writeCallback_ = callback;}
		void setErrorCallback(const EventCallback& callback) {errorCallback_ = callback;}
		void setCloseCallback(const EventCallback& callback) {closeCallback_ = callback;}
		void setRevents(int revents) {revents_ = revents;}
		void setIndex(int index) {index_ = index;}
		
		int fd()const {return fd_;}
		int events()const {return events_;}
		int index()const {return index_;}
		bool isNoneEvent()const {return events_ == kNoneEvent;}
		void disableAll() {events_ = kNoneEvent;update();}
		void enableReading() {events_ |= kReadEvent; update();}
		void enableWriting() {events_ |= kWriteEvent;update();}
		void disableWriting() {events_ &= ~kWriteEvent;update();}
		bool isWriting()const {return events_ & kWriteEvent;}
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
		bool eventHandling;
		readEventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback errorCallback_;
		EventCallback closeCallback_;
	};
}