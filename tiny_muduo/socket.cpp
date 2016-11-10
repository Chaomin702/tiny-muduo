#include "socket.h"
#include "socketOps.h"
#include <string.h>
#include "dbg.h"
using namespace cm;


cm::Socket::~Socket() {
	log_info("Socket %d closed", socketfd_);
	if (socketfd_ >= 0)
		cm::sockets::close(socketfd_);
}



void cm::Socket::bindAddress(const InetAddress& addr) {
	sockets::bind(socketfd_, addr.getSocketAddr());
}




void cm::Socket::listen() {
	sockets::listen(socketfd_);
}


int cm::Socket::accept(InetAddress *peeraddr) {
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	int confd = sockets::accept(socketfd_, &addr);
	if (confd > 0) {
		peeraddr->setSockAddrInet(addr);
	}
	return confd;
}


void cm::Socket::setReuseAddr(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof optval);
}


void cm::Socket::shutdownWrite() {
	sockets::shutdownWrite(socketfd_);
}


void cm::Socket::setTcpNoDelay(bool on) {
	int optval = on ? 1 : 0;
	int ret = ::setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof optval);
	if (ret < 0)
		log_err("Socket setTcpNoDelay failed");
}
