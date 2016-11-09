#include "timer.h"
using namespace cm;

void Timer::restart(TimeStamp now) {
	if (repeat_)
		expiration_ = addTime(now, interval_);
	else 
		expiration_ = TimeStamp::invalid();
}

