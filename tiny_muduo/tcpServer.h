#pragma once
#include "nonCopyable.h"
#include "acceptor.h"
#include "callback.h"
#include <map>
#include <string>
#include <memory>
#include "eventLoopThreadPoll.h"
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
		void setThreadNum(int num) {threadPoll_->setThreadNum(num);}
	private:
		using ConnectionMap = std::map<std::string, TcpConnetionPtr>;
		void newConnection(const Socket& connSocket, const InetAddress& addr);
		void removeConnection(const TcpConnetionPtr& cnn);
		void removeConnectionInLoop(const TcpConnetionPtr& cnn);
		EventLoop *loop_;
		bool started_;
		int nextConnId_;
		const std::string name_;
		std::unique_ptr<Acceptor> acceptor_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		ConnectionMap connections_;
		std::unique_ptr<EventLoopThreadPoll> threadPoll_;
	};
}