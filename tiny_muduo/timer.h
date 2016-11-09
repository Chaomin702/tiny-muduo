#pragma once
#include "timeStamp.h"
#include "nonCopyable.h"
#include "callback.h"
#include <string>
namespace cm
{
	class Timer :public NonCopyable {
	public:
		Timer(const TimerCallback& cb, TimeStamp when, double interval)
			: callback_(cb)
			, expiration_(when)
			, interval_(interval)
			, repeat_(interval > 0.0) {}
		void run()const {
			callback_();
		}
		TimeStamp expiration()const {return expiration_;}
		bool repeat()const {return repeat_;}
		void restart(TimeStamp now);
	private:
		const TimerCallback callback_;
		TimeStamp expiration_;
		const double interval_;
		const bool repeat_;
	};
}