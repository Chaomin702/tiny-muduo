#pragma once
#include <string>
#include <functional>
#include "nonCopyable.h"
#include "callback.h"
#include "tcpServer.h"
#include "inetAddress.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "timeWheel.h"
namespace cm {
	class EventLoop;
	class HttpServer :public NonCopyable {
	public:
		using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;
		HttpServer(EventLoop *loop, const InetAddress& listenAddr, int threadNum);
		void start();
		void setHomeDir(const std::string& dir) {homeDir_ = dir;}
		void setHttpCallback(const HttpCallback& cb) {httpCallback_ = cb;}
	private:
		void doRequest(const TcpConnetionPtr& conn, Buffer* buf, TimeStamp receiveTime);
		void onConnection(const TcpConnetionPtr& conn);
		void Response(const TcpConnetionPtr& conn, const HttpRequest& request);
		void onTimer();
		TcpServer server_;
		std::string homeDir_;
		HttpCallback httpCallback_;
		WeakConnectionList weakConnectionList_;
	};
}