#include "../include/SystemTasks.h"

#include "business_logic/DataSerializer/DataSerializer.h"

#include <iostream>

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
}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()));
	m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::edgeDetection, "edgeDetection", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(&m_taskParam), 4096);
	m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(commMng.get()), 4096);

}

void SystemTasks::edgeDetection(void* argument)
{
	auto taskArg = static_cast<TaskParams*>(argument);
	auto imageCapturer = taskArg->imageCapturer;//std::make_shared<business_logic::DataHandling::ImageCapturer>(*static_cast<business_logic::DataHandling::ImageCapturer*>(taskArg->imageCapturer.get()));
	auto sharedClkMng  = taskArg->sharedClkMng;
	const auto periodTimeCaptureImage = 2000;
	const auto delayCameraStartup     = 1000;

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
		imageCapturer->captureImage();
		const auto captureTimestamp = sharedClkMng->getTimeReference().toNs();
		m_dataHandlingTaskHandler->delayUntil(periodTimeCaptureImage);
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
			std::cout << "Failed to insert snapshot" << std::endl;
		}
	}
	/* USER CODE END 5 */
}

void SystemTasks::sendData(void* argument)
{
	auto commMng = std::make_shared<business_logic::Communication::CommunicationManager>(*static_cast<business_logic::Communication::CommunicationManager*>(argument));

	const auto sendDataPeriod = 200;

	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		std::cout << "Sending image information to master node" << std::endl;
		if(isPendingData())
		{
			business_logic::DataSerializer::ImageSnapshot nextSnapshot;
			getNextImage(nextSnapshot);
			for(size_t i = 0; i < (nextSnapshot.m_imgSize / MAXIMUN_CBOR_BUFFER_SIZE); i++)
		    {
				business_logic::DataSerializer::ImageSnapshot cborImgChunk{nextSnapshot.m_msgId, i, nextSnapshot.m_imgBuffer + (i*MAXIMUN_CBOR_BUFFER_SIZE), MAXIMUN_CBOR_BUFFER_SIZE, nextSnapshot.m_timestamp};
		    	std::vector<uint8_t> cborSerializedChunk;
		        m_dataSerializer->serialize(cborImgChunk, cborSerializedChunk);
		        const auto ptrSerializedMsg = cborSerializedChunk.data();
		        const auto serializedMsgSize = cborSerializedChunk.size();
		        if(serializedMsgSize > MAXIMUM_CAN_MSG_SIZE)
		        {
		        	std::cout << "Maximum CAN frame size" << std::endl;
		        }
		        std::vector<business_logic::Communication::CanMsg> canMsgChunks;
		        const auto cborIndex = (nextSnapshot.m_msgId << 6) | (i & 0x3F);
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

	const auto syncClkPeriod = 500;

	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
		std::cout << "Sync clk" << std::endl;
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

        for (size_t j = startIdx; j < endIdx; ++j)
        {
            canMsg.payload[j - startIdx] = cborSerializedChunk[j];
        }

        for (size_t j = endIdx - startIdx; j < payloadSize; ++j)
        {
            canMsg.payload[j] = 0;
        }

        canMsgChunks.push_back(canMsg);
    }
}

}
