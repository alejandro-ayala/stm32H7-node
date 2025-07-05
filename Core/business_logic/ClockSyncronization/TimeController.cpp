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
	LOG_INFO("TimeController::TimeControlle");
	internalTimer = new STM32Timer();
	BUSINESS_LOGIC_ASSERT( internalTimer != nullptr, services::BusinessLogicErrorId::TimeControllerInitialization, "TimeController initialization error");
	LOG_INFO("TimeController::TimeControlle created");

}

TimeController::~TimeController()
{
	LOG_INFO("TimeController::TimeControlle dtor");
	delete internalTimer;
};

void TimeController::initialize()
{
	restartTimer();
	clockOffset = 0;
	selfTest();
	initialized = true;
}

void TimeController::startTimer()
{
	BUSINESS_LOGIC_ASSERT( internalTimer != nullptr, services::BusinessLogicErrorId::TimeControllerInitialization, "TimeController::startTimer initialization error");
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

double TimeController::getCurrentNsecTime()
{
	BUSINESS_LOGIC_ASSERT( internalTimer != nullptr, services::BusinessLogicErrorId::TimeControllerInitialization, "TimeController::getCurrentNsecTime initialization error");
	return internalTimer->getCurrentNsec();

}
uint64_t TimeController::getCurrentTicks()
{
	BUSINESS_LOGIC_ASSERT( internalTimer != nullptr, services::BusinessLogicErrorId::TimeControllerInitialization, "TimeController::getCurrentTicks initialization error");
	return internalTimer->getCurrentTicks();
}

uint64_t TimeController::getElapsedTimeCounter() const
{
	return internalTimer->getElapsedTimeCounter();
}

uint64_t TimeController::getGlobalTime()
{
    // Tiempo local del esclavo + el desfase calculado con el maestro
    return getCurrentNsecTime() + clockOffset;
}

void TimeController::setClockOffset(int64_t newOffset)
{
	clockOffset = newOffset;
}

bool TimeController::selfTest()
{
	return true;

}
}
}
