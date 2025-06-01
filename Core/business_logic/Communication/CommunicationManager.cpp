
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

void CommunicationManager::sendData(const std::vector<business_logic::Communication::CanMsg>& dataToSend, bool sendEndOfImage)
{
    const uint8_t msgToSend = dataToSend.size();

    uint8_t canMsgId = 0;
    for(int currentMsgIndex = 0; currentMsgIndex < msgToSend; currentMsgIndex++)
    {

    	const auto& frame = dataToSend.at(currentMsgIndex);
        uint8_t data[MAXIMUM_CAN_MSG_SIZE] = {0};
        uint8_t i = 0;
        data[i++] = frame.canMsgId;
        data[i++] = frame.canMsgIndex;
        canMsgId = frame.canMsgId;
    	if(currentMsgIndex == (msgToSend - 1))
    	{
    		LOG_DEBUG("Last msg to send with a size: ", std::to_string(frame.payloadSize));
    	}
        uint8_t dataSize = CAN_MSG_ID_FIELD_SIZE + frame.payloadSize;
        for (; i < dataSize; i++)
        {
            data[i] = frame.payload[i - CAN_MSG_ID_FIELD_SIZE];
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
        std::string frameSize = "CanFrame of " + std::to_string(dataSize) + " bytes ";
        LOG_TRACE(frameSize, std::to_string(data[0]), " ", std::to_string(data[1]), " ", std::to_string(data[2]), " ", std::to_string(data[3]), " ",std::to_string(data[4]), " ", std::to_string(data[5]), " ", std::to_string(data[6]), " ", std::to_string(data[7]));
        canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::IMAGE_DATA), data, dataSize);
    }
    if(sendEndOfImage)LOG_DEBUG("CommunicationManager::sendData Sending end of Image");
//Send end of frame:
		uint8_t dataEndOfFrame[MAXIMUM_CAN_MSG_SIZE] = {0};
		dataEndOfFrame[0] = canMsgId;
		dataEndOfFrame[1] = 0x1E;
		dataEndOfFrame[2] = 0x1E;
		dataEndOfFrame[3] = 0x1E;
		dataEndOfFrame[4] = 0x1E;
		dataEndOfFrame[5] = 0x1E;
		dataEndOfFrame[6] = 0x1E;
		dataEndOfFrame[7] = sendEndOfImage;
		canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::IMAGE_DATA), dataEndOfFrame, MAXIMUM_CAN_MSG_SIZE);

}

bool CommunicationManager::waitingForConfirmation()
{
	uint8_t lenght = 800;
	uint8_t data[lenght];
	auto canMsg = canController->receiveMsg();
	auto msgSize = canMsg.dlc;
	bool frameConfirmed = false;
	if(msgSize > 0 && canMsg.id == static_cast<uint8_t>(CAN_IDs::FRAME_CONFIRMATION))
	{
		frameConfirmed = true;
	}
	return frameConfirmed;
}

bool CommunicationManager::selfTest()
{
	initialization();
	bool result = canController->selfTest();

	return result;
}
}
}
