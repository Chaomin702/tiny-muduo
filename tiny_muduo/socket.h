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
		int fd()const {return socketfd_;}
		void bindAddress(const InetAddress& addr);
		void listen();
		int accept(InetAddress *peeraddr);
		void setReuseAddr(bool on);
	private:
		const int socketfd_;
	};
}