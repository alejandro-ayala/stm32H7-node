
#include "CommunicationManager.h"
#include "CanIDs.h"
#include <iostream>

#include "../../hardware_abstraction/Controllers/CAN/CanController.h"

namespace business_logic
{
using namespace ClockSyncronization;
namespace Communication
{
CommunicationManager::CommunicationManager (std::shared_ptr<TimeController> timecontroller, std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller)  : timeController(timecontroller), canController(cancontroller)
{
}

CommunicationManager::~CommunicationManager()
{
}

void CommunicationManager::initialization()
{
	//canController->initialize();
}

void CommunicationManager::sendData(const std::vector<uint8_t>& msg)
{
    uint8_t bytesRemaining = msg.size();
    const uint8_t *msgPtr = msg.data();
    while (bytesRemaining > 0)
    {
        uint8_t bytesToSend = (bytesRemaining > 8) ? 8 : bytesRemaining;

        uint8_t data[8] = {0};
        for (uint8_t i = 0; i < bytesToSend; i++)
        {
            data[i] = msgPtr[i];
        }

        canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::IMAGE_DATA), data);

        msgPtr += bytesToSend;
        bytesRemaining -= bytesToSend;
    }
}

void CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	int msgSize = canController->receiveMsg(data);

	if(msgSize > 0)
	{
		//xil_printf("\n\rReceived data: %d bytes", msgSize);
//		IData parsedMsg;
//		parsedMsg.deSerialize(data);
//		std::cout << "newData[" << parsedMsg.secCounter << "]. sec: " << parsedMsg.timestamp << std::endl;
	}
}

bool CommunicationManager::selfTest()
{
	initialization();
	bool result = canController->selfTest();

	return result;
}
}
}
