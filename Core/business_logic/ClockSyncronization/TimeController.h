#pragma once

#include "ITimer.h"
#include "../../hardware_abstraction/Controllers/IController.h"

#include "TimeStamp.h"


namespace business_logic
{
namespace ClockSyncronization
{
class TimeController : public hardware_abstraction::Controllers::IController
{
private:
	ITimer* internalTimer;
	TimeStamp globalTimeStamp;
public:
	TimeController();
	~TimeController();

	void initialize() override;
	bool selfTest()   override;

	bool elapsedTime();
	void startTimer();
	void stopTimer();
	void restartTimer();
	void setPeriod(uint32_t period);
	double getCurrentSecTime();
	double getCurrentNsecTime();
	uint64_t getCurrentTicks();
	void setGlobalTimeReference(const TimeStamp& gt);
	TimeStamp getGlobalTimeReference();
	uint64_t getLocalTime();
};
}
}
