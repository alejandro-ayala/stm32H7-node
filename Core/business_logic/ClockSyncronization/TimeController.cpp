#include "../ClockSyncronization/TimeController.h"
#include "hardware_abstraction/Controllers/TIMER/STM32Timer.h"
#include "services/Logger/LoggerMacros.h"
#include "services/Exception/SystemExceptions.h"

using namespace hardware_abstraction::Controllers;
namespace business_logic
{
namespace ClockSyncronization
{
TimeController::TimeController() : IController("TimeController")
{
	internalTimer = new STM32Timer();
}

TimeController::~TimeController()
{
	delete internalTimer;
};

void TimeController::initialize()
{
	restartTimer();
	selfTest();
	initialized = true;
}

void TimeController::startTimer()
{
	internalTimer->startTimer();
}
void TimeController::stopTimer()
{
	internalTimer->stopTimer();
}

void TimeController::restartTimer()
{
	internalTimer->restartTimer();
}
//void TimeController::setPeriod(uint32_t period)
//{
//	internalTimer->setPeriod(period);
//}

double TimeController::getCurrentSecTime()
{
	return internalTimer->getCurrentSec();
}

double TimeController::getCurrentNsecTime()
{
	return internalTimer->getCurrentNsec();

}
uint64_t TimeController::getCurrentTicks()
{
	return internalTimer->getCurrentTicks();
}
//bool TimeController::elapsedTime()
//{
//	return internalTimer->elapsedTime();
//}
void TimeController::setGlobalTimeReference(const TimeStamp& gt)
{
	globalTimeStamp = gt;
	restartTimer();
}

TimeStamp TimeController::getGlobalTimeReference()
{
	return globalTimeStamp;
}

uint64_t TimeController::getLocalTime()
{
	uint64_t localNs = globalTimeStamp.toNs() + getCurrentNsecTime();
	return localNs;
}

bool TimeController::selfTest()
{
	double t1,t2,t3 ;
	internalTimer->restartTimer();
	t1   = internalTimer->getCurrentSec();
	HAL_Delay(1000);
	t2   = internalTimer->getCurrentSec();
	HAL_Delay(1000);
	t3   = internalTimer->getCurrentSec();
	if((((t2 - t1) < 0.9) || ((t2 - t1) > 1.1)) || (((t3 - t1) < 1.9) || ((t2 - t1) > 2.1)))
	{
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::TimeControllerInitialization, "TimeController::selfTest failed");
	}
	return true;
}
}
}
