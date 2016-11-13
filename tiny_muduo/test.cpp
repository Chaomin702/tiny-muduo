#include <iostream>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include <sys/timerfd.h>
#include <string.h>
#include "socket.h"
#include "acceptor.h"
#include "tcpServer.h"
#include "tcpConnection.h"
#include "httpServer.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
using namespace cm;
const std::string homedir = "/home/ubuntu/webserver/servfiles";
void onResponse(const HttpRequest& req, HttpResponse* response) {
	auto headers = req.headers();
	if (req.getMethod() == HttpRequest::kGet) {
		struct stat sbuf;
		const std::string path = homedir + req.getPath();
		if (stat(path.c_str(), &sbuf) == 0) {
			if ((S_ISREG(sbuf.st_mode)) && (S_IRUSR & sbuf.st_mode)) {
				int srcfd = open(path.c_str(), O_RDONLY, 0);
				char* scrp = (char*)mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
				close(srcfd);
				response->setBody(scrp, sbuf.st_size);
				munmap(scrp, sbuf.st_size);
				response->setStatusCode(HttpResponse::k200OK);
				response->setStatusMessage("OK");
				response->setContentType(req.getFileType());
			}else {
				response->setStatusCode(HttpResponse::k403Forbidden);
				response->setStatusMessage("Forbidden");  
			}
		}else {
			response->setStatusCode(HttpResponse::k404NotFound);
			response->setStatusMessage("Not found");
		}
	}else{
		response->setStatusCode(HttpResponse::k404NotFound);
		response->setStatusMessage("Not found");
	}
}

int main(int argc, char *argv[]){
	std::cout << "pid: " << getpid() << " tid: " << CurrentThread::tid() << "\n";
	InetAddress listenAddr(80);
	EventLoop loop;
	HttpServer server(&loop, listenAddr, 10);
	server.setHttpCallback(onResponse);
	server.start();
	loop.loop();
	return 0;
}