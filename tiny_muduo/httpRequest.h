#pragma once
#include <string>
#include <map>
#include <vector>
#include "timeStamp.h"
namespace cm {
	class HttpRequest {
	public:
		using string = std::string;
		enum Method { kUnknow = -1, kGet, kPost, kHead, kPut, kDelete };
		HttpRequest(): method_(kUnknow) {}
		bool setMethod(const char*b, const char*e) {
			string m(b, e);
			if (m == "GET")
				method_ = kGet;
			else if (m == "POST")
				method_ = kPost;
			else if (m == "HEAD")
				method_ = kHead;
			else if (m == "PUT")
				method_  = kPut;
			else if (m == "DELETE")
				method_ = kDelete;
			else
				method_ = kUnknow;
			return method_ != kUnknow;
		}
		Method getMethod()const {return method_;}
		void setPath(const char*b,const char* e) {
			path_.assign(b, e);
		}
		const string& getPath() {return path_;}
		void setReceiveTime(TimeStamp time) {receiveTime_ = time;}
		TimeStamp getReceiveTime()const {return receiveTime_;}
		void addHeader(const char*b, const char*colon,  const char*e) {
			while (isspace(*b))
				++b;
			string field(b, colon);
			++colon;
			while (colon!=e && isspace(*colon))
				++colon;
			string value(colon, e);
			headers_[field] = value;
		}
		string getHeader(const string& filed) const{
			string res;
			auto it = headers_.find(filed);
			if (it != headers_.end())
				res = it->second;
			return res;
		}
		void setFileType(const string& type) {
			fileType_ = type;
		}
		const string& getFileType() const {return fileType_;}
		void setCgiargs(const string& cgiargs) {cgiargs_ = cgiargs;}
		const string& getCgiargs() {return cgiargs_;}
		void swap(HttpRequest& rhs) {
			std::swap(path_, rhs.path_);
			std::swap(headers_, rhs.headers_);
			std::swap(method_, rhs.method_);
			std::swap(receiveTime_, rhs.receiveTime_);
		}
		void clear() {
			HttpRequest temp;
			swap(temp);
		}
	private:
		Method method_;
		string path_;
		std::map<string, string> headers_;
		TimeStamp receiveTime_;
		string fileType_;
		string cgiargs_;
	};	
}