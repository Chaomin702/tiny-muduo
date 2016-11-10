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
	return TimeStamp(seconds*kMicroSecondPerSecond+tv.tv_usec);
}

TimeStamp TimeStamp::invalid() {
	return TimeStamp();
}

std::string TimeStamp::toString()const {
	char buf[32] = { 0 };
	int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondPerSecond;
	int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondPerSecond;
	snprintf(buf,sizeof(buf)-1,"%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

std::string cm::TimeStamp::toFormattedString()const {
	  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondPerSecond);
  struct tm tm_time;
  localtime_r(&seconds, &tm_time);

  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
      microseconds);
  return buf;
}
