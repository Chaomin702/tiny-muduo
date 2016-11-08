#pragma once
#include <string>
namespace cm 
{
	class TimeStamp {
	public:
		TimeStamp();
		explicit TimeStamp(int64_t microSecondsSinceEpoch);
		void swap(TimeStamp& that) {
			std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
		}
		static TimeStamp now();
		static TimeStamp invalid();
		int64_t microSecondsSinceEpoch()const {return microSecondsSinceEpoch_;}
		static const int kMicroSencondPerSecond = 1000 * 1000;
	private:
		int64_t microSecondsSinceEpoch_;
	};
	inline bool operator<(const TimeStamp&lhs, const TimeStamp&rhs) {
		return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
	}
	inline bool operator==(const TimeStamp&lhs, const TimeStamp&rhs) {
		return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
	}
	inline TimeStamp addTime(TimeStamp timestamp, double seconds) {
		int64_t delta = static_cast<int64_t >(seconds*TimeStamp::kMicroSencondPerSecond);
		return TimeStamp(timestamp.microSecondsSinceEpoch() + delta);
	}
}