#include "inetAddress.h"
#include "strings.h"	//bzero
#include <arpa/inet.h>
#include "socketOps.h"
using namespace cm;
InetAddress::InetAddress(uint16_t port){
	bzero(&addr_, sizeof addr_);
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress(const std::string & ip, uint16_t port){
	bzero(&addr_, sizeof addr_);
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr));
}



std::string cm::InetAddress::toHostPort() const {
	char buf[32];
	sockets::toHostPort(buf, sizeof buf, addr_);
	return buf;
}
