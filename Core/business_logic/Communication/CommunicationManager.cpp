
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

void CommunicationManager::sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend)
{
    const uint8_t msgToSend = dataToSend.size();

    for(int currentMsgIndex = 0; currentMsgIndex < msgToSend; currentMsgIndex++)
    {
    	const auto& frame = dataToSend.at(currentMsgIndex);
        uint8_t data[MAXIMUM_CAN_MSG_SIZE] = {0};
        data[0] = frame.canMsgId;
        data[1] = frame.canMsgIndex;
        for (uint8_t i = ID_FIELD_SIZE; i < MAXIMUM_CAN_MSG_SIZE; i++)
        {
            data[i] = frame.payload[i - ID_FIELD_SIZE];
        }
        canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::IMAGE_DATA), data);
        //TODO check and remove the delay
        HAL_Delay(2);
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
