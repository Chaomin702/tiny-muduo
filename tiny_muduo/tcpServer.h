#pragma once
#include "nonCopyable.h"
#include "acceptor.h"
#include "callback.h"
#include <map>
#include <string>
#include <memory>
namespace cm {
	class EventLoop;
	class TcpServer : public NonCopyable {
	public:
		TcpServer(EventLoop *loop, const InetAddress& listenAddr);
		~TcpServer();
		void start();
		void setConnectionCallback(const ConnectionCallback&cb) {
			connectionCallback_ = cb;
		}
		void setMessageCallback(const MessageCallback&cb) {
			messageCallback_ = cb;
		}
	private:
		using ConnectionMap = std::map<std::string, TcpConnetionPtr>;
		void newConnection(const Socket& connSocket, const InetAddress& addr);
		void removeConnection(const TcpConnetionPtr& cnn);
		EventLoop *loop_;
		bool started_;
		int nextConnId_;
		const std::string name_;
		std::unique_ptr<Acceptor> acceptor_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		ConnectionMap connections_;
	};
}