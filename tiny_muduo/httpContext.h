#pragma once
#include "httpRequest.h"
namespace cm {
	class HttpContext {
	public:
		enum HttpRequestParseState { 
			kExpectRequestLine,
			kExpectHeaders,
			kExpectBody,
			kGotAll,
		};
		HttpContext():state_(kExpectRequestLine){}
		bool expectRequestLine()const {return state_ == cm::HttpContext::kExpectRequestLine;}
		bool expectHeaders()const {return state_ == cm::HttpContext::kExpectHeaders;}
		bool expectBody()const {return state_ == cm::HttpContext::kExpectBody;}
		bool GotAll()const {return state_ == cm::HttpContext::kGotAll;}
		void gotRequestLine() {state_ = cm::HttpContext::kExpectHeaders;}
		void gotHeaders() {state_ = cm::HttpContext::kExpectBody;}
		void gotBody() {state_ = cm::HttpContext::kGotAll;}
		void reset() {state_ = cm::HttpContext::kExpectRequestLine;request_.clear();}
		HttpRequest& request() {return request_;}
	private:
		HttpRequestParseState state_;
		HttpRequest request_;
	};
}
