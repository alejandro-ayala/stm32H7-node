#pragma once

#include "../../hardware_abstraction/Controllers/CAN/CanController.h"
#include "TimeController.h"
#include "TimeStamp.h"
#include <memory>

namespace business_logic
{
namespace ClockSyncronization
{
class SharedClockSlaveManager
{
private:
	std::shared_ptr<TimeController>                timeController;
	std::shared_ptr<hardware_abstraction::Controllers::CanController>    canController;
	TimeStamp                      globalTimeStamp;
	TimeBaseRef                    globalTimeReference;
public:
	SharedClockSlaveManager(std::shared_ptr<TimeController> timecontroller, std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller);
	~SharedClockSlaveManager();
	void initialization();
	bool getGlobalTime();
	TimeStamp getTimeReference() const;
};
}
}
