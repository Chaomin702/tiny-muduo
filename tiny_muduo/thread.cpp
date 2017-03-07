#include "thread.h"
#include "sys/syscall.h"
#include "linux/unistd.h"
#include "unistd.h"
#include <assert.h>
#include <iostream>
#include <pthread.h>
namespace cm 
{
namespace CurrentThread {
	__thread const char* ThreadName = "unknow";
	__thread pid_t CachedTid = 0;
}
}

using namespace cm;

pid_t gettid() {
	return static_cast<pid_t>(::syscall(SYS_gettid));
}
const char* cm::CurrentThread::name() {
	return ThreadName;
}
bool cm::CurrentThread::isMainThread() {
	return tid() == gettid();
}
pid_t cm::CurrentThread::tid() {
	if (CachedTid == 0)
		CachedTid = gettid();
	return CachedTid;
}


Thread::Thread(const ThreadFunc & callback):pthreadId_(0),tid_(0),isStarted_(false),callback_(callback){}

Thread::~Thread(){}

void Thread::setCallback(const ThreadFunc & callback){
	callback_ = callback;
}

void Thread::start(){
	assert(!isStarted_);
	isStarted_ = true;
	pthread_create(&pthreadId_, NULL, threadFunc, this); 
}

void Thread::join(){
	pthread_join(pthreadId_, NULL);
}

void * Thread::threadFunc(void *arg){
	Thread* pt = static_cast<Thread*>(arg);
	pt->runInThread();
	return NULL;
}

void Thread::runInThread() {
	tid_ = CurrentThread::tid();
	callback_();
}


