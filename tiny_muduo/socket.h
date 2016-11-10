#pragma once
#include "nonCopyable.h"
#include "inetAddress.h"
namespace cm 
{
	class Socket : public NonCopyable {
	public:
		explicit Socket(int fd)
			: socketfd_(fd) {}
		~Socket();
		Socket(Socket &&socket) noexcept
			:socketfd_(socket.socketfd_){
				socket.socketfd_ = -1;
			}
		void clear() {
			const_cast<int&>(socketfd_) = -1;
		}
		int fd()const {return socketfd_;}
		void bindAddress(const InetAddress& addr);
		void listen();
		int accept(InetAddress *peeraddr);
		void setReuseAddr(bool on);
		void shutdownWrite();
	private:
		int socketfd_;
	};
}