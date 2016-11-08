#include "threadPoll.h"
using namespace cm;

ThreadPoll::ThreadPoll(int pollSize)
	: cond_(mutex_)
	, pollSize_(pollSize)
	, isStarted_(false){}
ThreadPoll::~ThreadPoll(){
	if (isStarted_)
		stop();
}
void ThreadPoll::start(){
	assert(threads_.empty());
	if (isStarted_)
		return;
	isStarted_ = true;
	for (size_t i = 0; i < pollSize_; ++i){
		threads_.push_back(std::make_shared<Thread>(std::bind(&ThreadPoll::runInThread, this)));
	}
	for (auto &i : threads_){
		i->start();
	}
}

void ThreadPoll::addTask(const Task& t){
	MutexGuard lock(mutex_);
	queue_.push(t);
	cond_.notify();
}

ThreadPoll::Task ThreadPoll::getTask(){
	MutexGuard lock(mutex_);
	while (queue_.empty() && isStarted_){
		cond_.wait();
	}
	Task t;
	if (!queue_.empty()) {
		t = queue_.front();
		queue_.pop();
	 }
	return t;
}


void ThreadPoll::runInThread(){
	while (isStarted_){
		Task t = getTask();
		if(t)
			t();
	}
}

void ThreadPoll::stop(){
	isStarted_ = false;
	cond_.notifyAll();
	for (auto &i : threads_){
		i->join();
	}
}