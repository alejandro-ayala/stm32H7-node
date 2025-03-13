
#include "CommunicationManager.h"
#include "CanIDs.h"
#include "services/Logger/LoggerMacros.h"
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
        uint8_t dataSize = ID_FIELD_SIZE + frame.payloadSize;
        for (uint8_t i = ID_FIELD_SIZE - 1; i < MAXIMUM_CAN_MSG_SIZE; i++)
        {
            data[i] = frame.payload[i - ID_FIELD_SIZE + 1];
        }

/*******
        std::ostringstream messageStream;
        messageStream << "CAN Message: ";
        messageStream << "ID=" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(frame.canMsgId) << ", ";
        messageStream << "Index=" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(frame.canMsgIndex) << ", ";
        messageStream << "Payload=[";
        for (uint8_t i = 0; i < frame.payloadSize; i++)
        {
            if (i > 0)
                messageStream << " ";
            messageStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(frame.payload[i]);
        }
        messageStream << "]";

        // Convertir el stream a std::string
        std::string canMessage = messageStream.str();
        LOG_INFO(canMessage);

 *******/
        LOG_INFO("Sending frame: ", std::to_string(data[0]), " ", std::to_string(data[1]), " ", std::to_string(data[2]), " ", std::to_string(data[3]), " ",std::to_string(data[4]), " ", std::to_string(data[5]), " ", std::to_string(data[6]), " ", std::to_string(data[7]));
        canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::IMAGE_DATA), data, dataSize);
        //TODO check and remove the delay
        HAL_Delay(2);
    }
}

void CommunicationManager::receiveData()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	auto canMsg = canController->receiveMsg();
	auto msgSize = canMsg.dlc;
	if(msgSize > 0)
	{
		LOG_DEBUG("Received data:", msgSize, " bytes");
//		IData parsedMsg;
//		parsedMsg.deSerialize(data);
		//LOG_DEBUG("newData[", parsedMsg.secCounter, " sec: ", parsedMsg.timestamp);
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
