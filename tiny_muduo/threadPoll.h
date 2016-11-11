#pragma once
#include "nonCopyable.h"
#include "mutexLock.h"
#include "condition.h"
#include "thread.h"
#include <queue>
#include <functional>
#include <memory>
#include <vector>

namespace cm {
	
class ThreadPoll : private NonCopyable {
public:
	using Task = std::function<void()>;
	ThreadPoll(int pollSize);
	~ThreadPoll();
	void start();
	void stop();
	void addTask(const Task&);
	Task getTask();
private:
	void runInThread();
	mutable MutexLock mutex_;
	Condition cond_;
	std::queue<Task> queue_;
	int pollSize_;
	std::vector<std::shared_ptr<Thread>> threads_;
	bool isStarted_;
};
	
}