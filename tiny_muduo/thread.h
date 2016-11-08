#pragma once
#include <pthread.h>
#include <functional>
#include <iostream>
#include <functional>
#include "nonCopyable.h"
namespace cm {
	
class Thread : NonCopyable {
public:
	using ThreadFunc = std::function<void()>;
	explicit Thread(const ThreadFunc &callback);
	~Thread();
	pid_t pid()const {return tid_;}
	void setCallback(const ThreadFunc &callback);
	void start();
	void join();
private:
	static void* threadFunc(void *);	//why static? because of do not need this pointer?
	void runInThread();
	pthread_t pthreadId_;
	pid_t tid_;
	bool isStarted_;
	ThreadFunc callback_;
};
	
namespace CurrentThread
{
	pid_t tid();
	const char* name();
	bool isMainThread();
}
	
}