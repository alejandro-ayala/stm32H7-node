#include "../include/SystemTasks.h"

#include "business_logic/DataSerializer/DataSerializer.h"

#include "services/Logger/LoggerMacros.h"

namespace application
{

SystemTasks::SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	uint32_t queueItemSize   = sizeof(business_logic::DataSerializer::ImageSnapshot);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	createPoolTasks(commMng, imageCapturer, sharedClkMng);
	m_dataSerializer = std::make_shared<business_logic::DataSerializer::DataSerializer>();

	m_taskParam.imageCapturer = imageCapturer;
	m_taskParam.sharedClkMng  = sharedClkMng;

	//std::vector<uint8_t> cborSerializedChunk = {0xA4,0x69,0x69,0x6D,0x67,0x42,0x75,0x66,0x66,0x65,0x72,0x98,0x80,0x18,0x46,0x00,0x00,0x00,0x18,0x64,0x18,0xD9,0x01,0x18,0x24,0x18,0x43,0x18,0x41,0x18,0x4E,0x18,0x20,0x18,0x4D,0x18,0x65,0x18,0x73,0x18,0x73,0x18,0x61,0x18,0x67,0x18,0x65,0x18,0x3A,0x18,0x20,0x18,0x49,0x18,0x44,0x18,0x3D,0x18,0x30,0x18,0x30,0x18,0x2C,0x18,0x20,0x18,0x49,0x18,0x6E,0x18,0x64,0x18,0x65,0x18,0x78,0x18,0x3D,0x18,0x32,0x18,0x61,0x18,0x2C,0x18,0x20,0x18,0x50,0x18,0x61,0x18,0x79,0x18,0x6C,0x18,0x6F,0x18,0x61,0x18,0x64,0x18,0x3D,0x18,0x5B,0x18,0x30,0x18,0x30,0x18,0x5D,0x00,0x18,0x30,0x12,0x00,0x00,0x00,0x18,0xE8,0x18,0xDA,0x01,0x18,0x24,0x18,0x30,0x18,0x20,0x18,0x38,0x18,0x36,0x18,0x5D,0x00,0x01,0x18,0x24,0x00,0x00,0x00,0x00,0x18,0x40,0x00,0x00,0x00,0x10,0x18,0xD4,0x01,0x18,0x24,0x18,0xD8,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0xE0,0x00,0x10,0x18,0x18,0x18,0x4A,0x18,0x18,0x18,0x46,0x18,0x18,0x18,0x49,0x18,0x18,0x18,0x46,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x18,0x18,0x18,0xFF,0x18,0x18,0x10,0x18,0xD4,0x01,0x18,0x24,0x18,0x18,0x00,0x00,0x00,0x10,0x18,0xD4,0x01,0x18,0x24,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x67,0x69,0x6D,0x67,0x53,0x69,0x7A,0x65,0x18,0x80,0x65,0x6D,0x73,0x67,0x49,0x64,0x00,0x69,0x74,0x69,0x6D,0x65,0x73,0x74,0x61,0x6D,0x70,0x1A,0x09,0x70,0x86,0x00};
	//std::vector<uint8_t> memory1 = {0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0xFF,0xC4,0x00,0xB5,0x10,0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D,0x01,0x02,0x03,0x00,
	//		0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,0x24,0x33,0x62,0x72,
	//		0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,
	//		0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94};

	std::vector<uint8_t> memory0 = {0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46,0x00,0x01,0x01,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0xFF,0xDB,0x00,0x43,0x00,0x20,0x16,0x18,0x1C,0x18,0x14,0x20,0x1C,0x1A,0x1C,0x24,0x22,0x20,0x26,0x30,0x50,0x34,0x30,0x2C,0x2C,0x30,0x62,0x46,0x4A,0x3A,0x50,0x74,0x66,0x7A,0x78,0x72,0x66,0x70,0x6E,0x80,0x90,0xB8,0x9C,0x80,0x88,0xAE,0x8A,0x6E,0x70,0xA0,0xDA,0xA2,0xAE,0xBE,0xC4,0xCE,0xD0,0xCE,0x7C,0x9A,0xE2,0xF2,0xE0,0xC8,0xF0,0xB8,0xCA,0xCE,0xC6,0xFF,0xC0,0x00,0x0B,0x08,0x00,0xF0,0x01,0x40,0x01,0x01,0x11,0x00,0xFF,0xC4,0x00,0x1F,0x00,0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04};
	business_logic::DataSerializer::ImageSnapshot cborImgChunk{0, 0, memory0.data(), 128, 0x1234};
	std::vector<uint8_t> cborSerializedChunk;
    m_dataSerializer->serialize(cborImgChunk, cborSerializedChunk);

    business_logic::DataSerializer::ImageSnapshot cborImgChunk2;
	m_dataSerializer->deserialize(cborImgChunk2, cborSerializedChunk);
	int a = 0;

}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	//m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()));
	m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::edgeDetection, "edgeDetection", DefaultPriorityTask + 1, static_cast<business_logic::Osal::VoidPtr>(&m_taskParam), 4096);
	m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(commMng.get()), 4096);

}

void SystemTasks::edgeDetection(void* argument)
{
	auto taskArg = static_cast<TaskParams*>(argument);
	auto imageCapturer = taskArg->imageCapturer;//std::make_shared<business_logic::DataHandling::ImageCapturer>(*static_cast<business_logic::DataHandling::ImageCapturer*>(taskArg->imageCapturer.get()));
	auto sharedClkMng  = taskArg->sharedClkMng;
	const auto periodTimeCaptureImage = 30000;
	const auto delayCameraStartup     = 1000;
	LOG_INFO("SystemTasks::edgeDetection started");
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();

	imageCapturer->initialize();
	m_dataHandlingTaskHandler->delayUntil(delayCameraStartup);
	const auto imgBufferSize = imageCapturer->getBufferSize();
	auto edges   = std::make_shared<std::vector<uint8_t>>(imgBufferSize);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
    freeHeapSize = xPortGetFreeHeapSize();
    minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
	for(;;)
	{
		LOG_INFO("SystemTasks::edgeDetection captureImage");
		imageCapturer->captureImage();
		const auto captureTimestamp = sharedClkMng->getTimeReference().toNs();
		imageCapturer->extractImage();

		const uint8_t* rawImgBuffer = imageCapturer->getRawImageBuffer();
		size_t bufferSize = imageCapturer->getRawImageBufferSize();

		freeHeapSize = xPortGetFreeHeapSize();
		minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();

		auto edgesPtr = edges->data();
		const auto edgeCompressedImgSize = imageCapturer->processEdges(rawImgBuffer, edgesPtr, bufferSize);
		static uint8_t captureId = 0;
		business_logic::DataSerializer::ImageSnapshot edgesSnapshot{captureId, 0x00, edgesPtr, edgeCompressedImgSize, captureTimestamp};
		captureId++;
		if(captureId == 4) captureId = 0;
		const auto isInserted = m_capturesQueue->sendToBack(( void * ) &edgesSnapshot);
		if(!isInserted)
		{
			LOG_ERROR("Failed to insert snapshot");
		}
		m_dataHandlingTaskHandler->delay(periodTimeCaptureImage);
	}
	/* USER CODE END 5 */
}

void SystemTasks::sendData(void* argument)
{
	auto commMng = std::make_shared<business_logic::Communication::CommunicationManager>(*static_cast<business_logic::Communication::CommunicationManager*>(argument));

	const auto sendDataPeriod = 1000;
	LOG_INFO("SystemTasks::sendData started");
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		if(isPendingData())
		{
			LOG_INFO("Sending image information to master node");
			business_logic::DataSerializer::ImageSnapshot nextSnapshot;
			getNextImage(nextSnapshot);
			for(size_t i = 0; i < (nextSnapshot.m_imgSize / MAXIMUN_CBOR_BUFFER_SIZE); i++)
		    {
				business_logic::DataSerializer::ImageSnapshot cborImgChunk{nextSnapshot.m_msgId, i, nextSnapshot.m_imgBuffer + (i*MAXIMUN_CBOR_BUFFER_SIZE), MAXIMUN_CBOR_BUFFER_SIZE, nextSnapshot.m_timestamp};
		    	const auto ptrImgChunkBuffer = cborImgChunk.m_imgBuffer;
				std::vector<uint8_t> cborSerializedChunk;
		        m_dataSerializer->serialize(cborImgChunk, cborSerializedChunk);
		        const auto ptrSerializedMsg = cborSerializedChunk.data();
		        const auto serializedMsgSize = cborSerializedChunk.size();
		        if(serializedMsgSize > MAXIMUM_CAN_MSG_SIZE)
		        {
		        	LOG_WARNING("Sending image information to master node");
		        }

		        std::vector<business_logic::Communication::CanMsg> canMsgChunks;
		        const auto cborIndex = (nextSnapshot.m_msgId << 6) | (i & 0x3F);
		        /********************/
				std::cout << "cborSerializedChunk (hex): ";
				for (const auto& byte : cborSerializedChunk) {
					std::cout << std::hex << std::setw(2) << std::setfill('0')
							  << static_cast<int>(byte) << " ";
				}
				std::cout << std::dec << std::endl;  // Restaurar formato decimal
		        /****************************/
		        splitCborToCanMsgs(cborIndex, cborSerializedChunk, canMsgChunks);
				commMng->sendData(canMsgChunks);
		    }
		}

		m_commTaskHandler->delayUntil(sendDataPeriod);
	}
	/* USER CODE END 5 */
}

void SystemTasks::syncronizationGlobalClock(void* argument)
{
	auto sharedClkMng = std::make_shared<business_logic::ClockSyncronization::SharedClockSlaveManager>(*static_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument));

	const auto syncClkPeriod = 15000;
	LOG_INFO("SystemTasks::syncronizationGlobalClock started");
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		LOG_INFO("SystemTasks::syncronizationGlobalClock");
		m_clockSyncTaskHandler->delayUntil(syncClkPeriod);
	}
	/* USER CODE END 5 */
}

bool SystemTasks::isPendingData()
{
	return (m_capturesQueue->getStoredMsg() > 0);
}

void SystemTasks::getNextImage(business_logic::DataSerializer::ImageSnapshot& edgesSnapshot)
{
	m_capturesQueue->receive(&edgesSnapshot);
}

void SystemTasks::splitCborToCanMsgs(uint8_t canMsgId, const std::vector<uint8_t>& cborSerializedChunk, std::vector<business_logic::Communication::CanMsg>& canMsgChunks)
{
    size_t totalBytes = cborSerializedChunk.size();
    size_t payloadSize = MAXIMUM_CAN_MSG_SIZE - 2;
    size_t numberOfMsgs = (totalBytes + payloadSize - 1) / payloadSize;

    for (size_t i = 0; i < numberOfMsgs; ++i)
    {
        business_logic::Communication::CanMsg canMsg;

        canMsg.canMsgId =canMsgId;
        canMsg.canMsgIndex = static_cast<uint8_t>(i);
        size_t startIdx = i * payloadSize;
        size_t endIdx = std::min(startIdx + payloadSize, totalBytes);
        canMsg.payloadSize = endIdx - startIdx;

        for (size_t j = startIdx; j < endIdx; ++j)
        {
            canMsg.payload[j - startIdx] = cborSerializedChunk[j];
        }
        canMsgChunks.push_back(canMsg);
    }
}

}
