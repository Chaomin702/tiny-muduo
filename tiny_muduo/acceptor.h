#pragma once
#include <functional>
#include "nonCopyable.h"
#include "socket.h"
#include "channel.h"
namespace cm
{
	class EventLoop;
	class InetAddress;
	class Acceptor : public NonCopyable {
	public:
		using NewConnectionCallback = std::function<void(const Socket &connSocket, const InetAddress&)>;
		Acceptor(EventLoop *loop, const InetAddress&addr);
		void setNewConnectionCallback(const NewConnectionCallback& cb) {
			newConnectionCallback_ = cb;
		}
		bool listening()const {return listenning_;}
		void listen();
	private:
		void handleRead();
		bool listenning_;
		EventLoop *loop_;
		Socket acceptSocket_;
		Channel acceptChannel_;
		NewConnectionCallback newConnectionCallback_;
	};
}