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
		bool valid()const {return microSecondsSinceEpoch_ > 0;}
		std::string toString()const;
		std::string toFormattedString()const;
		int64_t microSecondsSinceEpoch()const {return microSecondsSinceEpoch_;}
		static const int kMicroSecondPerSecond = 1000 * 1000;
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
		int64_t delta = static_cast<int64_t >(seconds*TimeStamp::kMicroSecondPerSecond);
		return TimeStamp(timestamp.microSecondsSinceEpoch() + delta);
	}
}