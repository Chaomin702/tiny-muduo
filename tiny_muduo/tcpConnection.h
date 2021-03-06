#pragma once
#include "nonCopyable.h"
#include "inetAddress.h"
#include <memory>
#include "callback.h"
#include <string>
#include "socket.h"
#include "channel.h"
#include "buffer.h"
#include "any.h"
namespace cm 
{
	class EventLoop;
	class TcpConnection :public NonCopyable, public std::enable_shared_from_this<TcpConnection> {
	public:
		TcpConnection(EventLoop *loop, const std::string name,
			const Socket& sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
		~TcpConnection();
		EventLoop* getloop()const {return loop_;}
		bool connected()const {return state_ == kConnected;}
		const std::string& name()const {return name_;}
		const InetAddress& localAddrress()const {return localAddr_;}
		const InetAddress& peerAddress()const {return peerAddr_;}
		void setConnectionCallback(const ConnectionCallback&cb) {
			connectionCallback_ = cb;
		}
		void setMessageCallback(const MessageCallback&cb) {
			messageCallback_ = cb;
		}
		void setCloseCallback(const CloseCallback&cb) {
			closeCallback_ = cb;
		}
		void connectionEstablished();
		void connectionDestroyed();
		void send(const std::string& message);
		void send(const void* data, size_t len);
		void send(Buffer *buf);
		void shutdown();
		void setContext(const Any& context) {contest_ = context;}
		Any& getContext() {return contest_;}
		const Any& getContext()const {return contest_;}
		void setEntry(const Any& entry) {entry_ = entry;}
		Any& getEntry() {return entry_;}
		const Any& getEntry()const {return entry_;}
		void activeClose() {handleClose();}
	private:
		enum StateE { kConnecting, kConnected, kDisconnecting ,kDisconnected,};
		void setState(StateE s) {state_ = s;}
		void handleRead(TimeStamp receiveTime);
		void handleWrite();
		void handleClose();
		void handleError();
		void sendInLoop(const std::string& message);
		void sendInLoop(const void* data, size_t len);
		void shutdownInLoop();
		EventLoop *loop_;
		std::string name_;
		StateE state_;
		std::unique_ptr<Socket> socket_;
		std::unique_ptr<Channel> channel_;
		InetAddress localAddr_;
		InetAddress peerAddr_;
		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;
		CloseCallback closeCallback_;
		Buffer inputBuffer_;
		Buffer outputBuffer_;
		Any contest_;
		Any entry_;
	};
	using TcpConnetionPtr = std::shared_ptr<TcpConnection>;
}