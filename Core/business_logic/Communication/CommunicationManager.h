#pragma once

#include "business_logic/DataSerializer/ImageSnapshot.h"
#include "business_logic/Osal/QueueHandler.h"
#include "../../hardware_abstraction/Controllers/CAN/CanController.h"
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
	void sendData(const std::vector<uint8_t>& msg);
	void receiveData(void);
	bool selfTest();

};
}
}
