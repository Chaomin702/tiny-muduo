#include "httpResponse.h"
#include "buffer.h"
#include <stdio.h>
using namespace cm;

void cm::HttpResponse::appendToBuffer(Buffer* output)const {
	char buf[32];
	snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
	output->append(buf).append(statusMessage_).append("\r\n");
	if (closeConnection_)
		output->append("Connection: close\r\n");
	else {
		snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
		output->append(buf);
	}
	for (auto&i : headers_) {
		output->append(i.first).append(": ").append(i.second).append("\r\n");
	}
	output->append("\r\n");
	output->append(body_);
}
