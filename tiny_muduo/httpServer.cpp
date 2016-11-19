#include "httpServer.h"
#include "httpContext.h"
#include "tcpConnection.h"
#include "eventLoop.h"
#include "dbg.h"
#include "thread.h"
using namespace cm;
using namespace std::placeholders;
const std::map<const std::string, const std::string> httpFileTypes = {
	{".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/msword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"},
    {"" ,"text/plain"}
	};
bool parseRequestLine(const char*b, const char*e, HttpContext* context) {
	bool succeed = false;
	while (b != e && isspace(*b)) 
		++b;
	const char * space = std::find(b, e, ' ');
	if (space != e && context->request().setMethod(b, space)) {
		b = space + 1;
		space = std::find(b, e, ' ');
		if (space != e ) {
			context->request().setPath(b, space);
			b = space + 1;
			succeed = std::equal(b, e, "HTTP/1.1");
		}
	}
	return succeed;
}
const std::string& getFileType(const std::string& type) {
	auto it = httpFileTypes.find(type);
	if (it != httpFileTypes.end())
		return it->second;
	else
		return httpFileTypes.find("")->second;
}
bool parseUri(HttpRequest& req) {
	std::string path = req.getPath(), filetype, cgiargs;
	auto mark = std::find(path.begin(), path.end(), '?');
	if (mark != path.end()) {
		cgiargs.assign(mark + 1, path.end());
		path.erase(mark, path.end());
	}
	if (path.back() == '/')
		path.append("index.html");
	size_t dot = path.find_last_of('.');
	if (dot == std::string::npos)
		return false;
	filetype = getFileType(path.substr(dot));
	req.setPath(path);
	req.setCgiargs(cgiargs);
	req.setFileType(filetype);
	return true;
}

bool parseRequest(Buffer* buf, HttpContext* context, TimeStamp receiveTime) {
	bool ok = false;
	bool hasMore = true;
	while (hasMore) {
		if (context->expectRequestLine()) {
			const char* crlf = buf->findCRLF();
			if (crlf) {
				log_info("request line: %s", std::string().assign(buf->peek(),crlf).c_str());
				ok = parseRequestLine(buf->peek(), crlf, context);
				if (ok) {
					context->request().setReceiveTime(receiveTime);
					buf->retrieveUntil(crlf + 2);
					context->gotRequestLine();
				}else
					hasMore = false;
			}else
				hasMore = false;
		}else if(context->expectHeaders()){
			const char* crlf = buf->findCRLF();
			if (crlf) {
				const char* colon = std::find(buf->peek(), crlf, ':');
				if (colon!=crlf) {
					context->request().addHeader(buf->peek(), colon, crlf);
				}else{
					context->gotHeaders();
				}
				buf->retrieveUntil(crlf + 2);
			}else
				hasMore = false;
		}else if(context->expectBody()){
			context->gotBody();
		}else if(context->GotAll()){
			hasMore = false;
		}
	}
	return ok;
}
	
HttpServer::HttpServer(EventLoop *loop, const InetAddress& listenAddr, int threadNum)
	: server_(loop, listenAddr)
	, weakConnectionList_(8){
		server_.setThreadNum(threadNum);
		server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
		server_.setMessageCallback(std::bind(&HttpServer::doRequest, this, _1, _2, _3));
		loop->runEvery(1.0, std::bind(&HttpServer::onTimer, this));
	}

void cm::HttpServer::start() {
	log_info("HTTP server started");
	server_.start();
}
void HttpServer::doRequest(const TcpConnetionPtr& conn, Buffer* buf, TimeStamp receiveTime) {
	WeakEntryPtr weakEntry(anyCast<WeakEntryPtr>(conn->getEntry()));
	EntryPtr entry(weakEntry.lock());
	if (entry) {
		weakConnectionList_.back().insert(entry);	
	}else{
		log_info("you should be here!");
	}
	
	HttpContext* context = anyCast<HttpContext>(&conn->getContext());
	if (!parseRequest(buf, context, receiveTime)) {
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
		context->reset();
	}
	if (context->GotAll()) {
		if (!parseUri(context->request())) {
			conn->send("HTTP/1.1 400 invalid url\r\n\r\n");
			conn->shutdown();
		}
		else {
			Response(conn, context->request());
		 }
		context->reset();
	}
}
void HttpServer::onConnection(const TcpConnetionPtr& conn) {
	if (conn->connected()) {
		conn->setContext(HttpContext());
		log_info("new request comming from %s, %s thread  %d handle it at %s",
			conn->peerAddress().toHostPort().c_str(), 
			conn->name().c_str(), CurrentThread::tid(),
			TimeStamp::now().toFormattedString().c_str());
		//for timewheel
		EntryPtr entry(new Entry(conn));
		weakConnectionList_.back().insert(entry);
		WeakEntryPtr weakEntry(entry);
		conn->setEntry(weakEntry);
	}else{
		WeakEntryPtr weakEntry(anyCast<WeakEntryPtr>(conn->getEntry()));
	}
}

void cm::HttpServer::Response(const TcpConnetionPtr& conn, const HttpRequest& request) {
	HttpResponse response(false);
	httpCallback_(request, &response);
	Buffer buf;
	response.appendToBuffer(&buf);
	conn->send(&buf);
//	conn->shutdown();
}


void cm::HttpServer::onTimer() {
	weakConnectionList_.push_back(Bucket());
}
