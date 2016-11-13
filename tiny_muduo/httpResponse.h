#pragma once
#include "nonCopyable.h"
#include <map>
#include <string>
namespace cm {
	class Buffer;
	class HttpResponse {
	public:
		using string = std::string;
		enum HttpStatusCode { 
			kUnknow,
			k200OK = 200,
			k301MovedPermanently = 301,
			k400BadRequest = 400,
			k403Forbidden = 403,
			k404NotFound = 404,
		};
		HttpResponse(bool close)
			: statusCode_(kUnknow)
			, closeConnection_(close) {}
		void setStatusCode(HttpStatusCode code) {statusCode_ = code;}
		void setStatusMessage(const string& message) {statusMessage_ = message;}
		void setContentType(const string& contentType) {
			addHeader("Content-Type", contentType);
		}
		void addHeader(const string& key, const string& value) {
			headers_[key] = value;
		}
		void appendToBuffer(Buffer* output)const;
		void setCloseConnection(bool close) {closeConnection_ = close;}
		void setBody(const string& body) {
			body_ = body;
		}
		void setBody(const void* body, size_t len) {
			body_.assign(static_cast<const char*>(body), len);
		}
	private:
		HttpStatusCode statusCode_;
		bool closeConnection_;
		std::map<string, string> headers_;
		string statusMessage_;
		string body_;
	};
}