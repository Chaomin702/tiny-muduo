#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <assert.h>

namespace cm 
{
	class Buffer {
	public:
		static const size_t kInitialSize = 1024;
		static const size_t kCheapPrepend = 8;
		Buffer()
			: buffer_(kCheapPrepend + kInitialSize)
			, readerIndex_(kCheapPrepend)
			, writerIndex_(kCheapPrepend) {
				assert(readableBytes() == 0);
				assert(writableBytes() == kInitialSize);
				assert(prependableBytes() == kCheapPrepend);
			}
		void swap(Buffer& rhs) {
			buffer_.swap(rhs.buffer_);
			std::swap(readerIndex_, rhs.readerIndex_);
			std::swap(writerIndex_, rhs.writerIndex_);
		}
		size_t readableBytes()const {return writerIndex_ - readerIndex_;}
		size_t writableBytes()const {return buffer_.size() - writerIndex_;}
		size_t prependableBytes()const {return readerIndex_;}
		const char* peek()const {return begin() + readerIndex_;}
		void retrieve(size_t len) {
			assert(len <= readableBytes());
			readerIndex_ += len;
		}
		void retrieveUntil(const char *end) {
			assert(peek() <= end);
			assert(end <= beginWrite());
			retrieve(end - peek());
		}
		void retrieveAll() {
			readerIndex_ = kInitialSize;
			writerIndex_ = kInitialSize;
		}
		std::string retrieveAsString() {
			std::string str(peek(), readableBytes());
			retrieveAll();
			return str;
		}
		Buffer& append(const std::string& str) {
			return append(str.data(), str.length());
		}
		Buffer& append(const char* data, size_t len) {
			ensureWritableBytes(len);
			std::copy(data, data + len, beginWrite());
			hasWritten(len);
			return *this;
		}
		Buffer& append(const void* data, size_t len) {
			return append(static_cast<const char*>(data), len);
		}
		void ensureWritableBytes(size_t len) {
			if (writableBytes() < len) {
				makeSpace(len);
			}
			assert(writableBytes() >= len);
		}
		const char* findCRLF()const {
			const char* p = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
			return p == beginWrite() ? NULL : p;
		}
		const char* findCRLF(const char* start)const {
			assert(peek() <= start);
			assert(start <= beginWrite());
			const char* p = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
			return p == beginWrite() ? NULL : p;
		}
		char* beginWrite() {return begin() + writerIndex_;}
		const char* beginWrite()const {return begin() + writerIndex_;}
		void hasWritten(size_t len) {writerIndex_ += len;}
		void prepend(const void*data, size_t len) {
			assert(len <= prependableBytes());
			readerIndex_ -= len;
			const char* d = static_cast<const char*>(data);
			std::copy(d, d + len, begin() + readerIndex_);
		}
		ssize_t readFd(int fd, int*savedErrno);
	private:
		static const char kCRLF[];
		char* begin() {return & *buffer_.begin();}
		const char* begin()const {return & *buffer_.begin();}
		void  makeSpace(size_t len) {
			if (writableBytes() + prependableBytes() < len) {
				buffer_.resize(writerIndex_ + len);
			}else{
				assert(kCheapPrepend < readerIndex_);
				size_t readable = readableBytes();
				std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
				readerIndex_ = kCheapPrepend;
				writerIndex_ = readerIndex_ + readable;
				assert(readable == readableBytes());
			}
		}
		std::vector<char> buffer_;
		size_t readerIndex_;
		size_t writerIndex_;
	};
}