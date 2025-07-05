#pragma once

#include "../../hardware_abstraction/Controllers/Timer/ITimer.h"
#include "../../hardware_abstraction/Controllers/IController.h"

#include "TimeStamp.h"


namespace business_logic
{
namespace ClockSyncronization
{
class TimeController : public hardware_abstraction::Controllers::IController
{
private:
	hardware_abstraction::Controllers::ITimer* internalTimer;
	TimeStamp globalTimeStamp;
	int64_t clockOffset;
public:
	TimeController();
	~TimeController();

	void initialize() override;
	bool selfTest()   override;

	bool elapsedTime();
	void startTimer();
	void stopTimer();
	void restartTimer();
	void setClockOffset(int64_t newOffset);
	void setPeriod(uint32_t period);
	double getCurrentSecTime();
	double getCurrentNsecTime();
	uint64_t getCurrentTicks();
	void setGlobalTimeReference(const TimeStamp& gt);
	TimeStamp getGlobalTimeReference();
	uint64_t getLocalTime();
	uint64_t getElapsedTimeCounter() const;
	uint64_t getGlobalTime();
};
}
}
