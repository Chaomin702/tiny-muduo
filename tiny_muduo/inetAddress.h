#pragma once
#include <netinet/in.h>
#include <string>
namespace cm 
{
	class InetAddress {
	public:
		explicit InetAddress(uint16_t port);
		explicit InetAddress(const struct sockaddr_in& addr)
			: addr_(addr) {}
		InetAddress(const std::string& ip, uint16_t port);
		const struct sockaddr_in& getSocketAddr()const { return addr_; }
		void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }
		std::string toHostPort() const;
	private:
		struct sockaddr_in addr_;
	};
}
