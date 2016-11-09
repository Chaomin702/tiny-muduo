#include "socketOps.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "dbg.h"
#include "errno.h"
typedef struct sockaddr SA;
using namespace cm;
SA* sockaddr_cast(struct sockaddr_in *addr) {
	return static_cast<SA*>((void*)(addr));
}

const SA* sockaddr_cast(const struct sockaddr_in *addr) {
	return static_cast<SA*>((void*)(addr));
}

int sockets::createNonblocking() {
	int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
		log_err("createNonblocking");
	setNonBlockAndCloseOnExec(sockfd);
	return sockfd;
}

void sockets::setNonBlockAndCloseOnExec(int sockfd){
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);
	if (ret == -1)
		log_err("setNonBlockAndCloseOnExec");
	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);
	if (ret == -1)
		log_err("setNonBlockAndCloseOnExec");
}


void sockets::bind(int sockfd, const struct sockaddr_in & addr){
	int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof addr);
	if (ret < 0)
		log_err("sockets::bind");
}

void sockets::listen(int sockfd){
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0)
		log_err("sockets::listen");
}

int sockets::accept(int sockfd, sockaddr_in * addr){
	socklen_t addrlen = sizeof *addr;
	int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
	if (connfd < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			log_err("sockets::accept");
	}
	return connfd;
}

void sockets::close(int sockfd){
	if (::close(sockfd) < 0)
		log_err("sockets::close");
	log_info("socket fd %d closed", sockfd);
}


void cm::sockets::toHostPort(char* buf, size_t size, const struct sockaddr_in& addr) {
	char host[INET_ADDRSTRLEN] = "INVALID";
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
	uint16_t port = ::ntohs(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}
