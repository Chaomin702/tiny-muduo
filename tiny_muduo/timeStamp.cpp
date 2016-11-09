#include "timeStamp.h"
#include <sys/time.h>
#include <inttypes.h>
using namespace cm;

TimeStamp::TimeStamp()
	: microSecondsSinceEpoch_(0) {}

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
	: microSecondsSinceEpoch_(microSecondsSinceEpoch) {}

TimeStamp TimeStamp::now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return TimeStamp(seconds*kMicroSencondPerSecond+tv.tv_usec);
}

TimeStamp TimeStamp::invalid() {
	return TimeStamp();
}

std::string TimeStamp::toString()const {
	char buf[32] = { 0 };
	int64_t seconds = microSecondsSinceEpoch_ / kMicroSencondPerSecond;
	int64_t microseconds = microSecondsSinceEpoch_ % kMicroSencondPerSecond;
	snprintf(buf,sizeof(buf)-1,"%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}