#pragma once

#include "../../hardware_abstraction/Controllers/CAN/CanController.h"
#include "IData.h"
#include "../ClockSyncronization/TimeStamp.h"
#include "../ClockSyncronization/TimeController.h"
#include <memory>
namespace business_logic
{
namespace Communication
{
class CommunicationManager
{
private:
	std::shared_ptr<ClockSyncronization::TimeController>                timeController;
	std::shared_ptr<hardware_abstraction::Controllers::CanController>    canController;
	ClockSyncronization::TimeStamp       globalTimeStamp;
public:
	CommunicationManager(std::shared_ptr<ClockSyncronization::TimeController> timecontroller, std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller);
	virtual ~CommunicationManager();

	void initialization();
	void sendData(IData msg);
	void receiveData(void);
	bool selfTest();

};
}
}
