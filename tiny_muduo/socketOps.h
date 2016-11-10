#pragma once
#include <netinet/in.h>
#include <sys/signal.h>
#include <string>
namespace cm {
	namespace sockets {
		int createNonblocking();
		void setNonBlockAndCloseOnExec(int sockfd);
		void bind(int sockfd, const struct sockaddr_in& addr);
		void listen(int sockfd);
		int accept(int sockfd, struct sockaddr_in *addr);
		void close(int sockfd);
		void toHostPort(char* buf, size_t size, const struct sockaddr_in& addr);
		struct sockaddr_in getLocalAddr(int sockfd);
	}
}