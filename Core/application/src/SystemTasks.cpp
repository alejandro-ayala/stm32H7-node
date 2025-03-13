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

	LOG_INFO(" Initial value of queue: ", std::to_string(isPendingData()));
}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	try {
		m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask +2, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()), 4096);
		m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::edgeDetection, "edgeDetection", DefaultPriorityTask + 1, static_cast<business_logic::Osal::VoidPtr>(&m_taskParam), 4096);
		m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(commMng.get()), 4096);

	} catch (...)
	{
		LOG_ERROR("SystemTasks::createPoolTasks FAILED");
	}

}

void SystemTasks::edgeDetection(void* argument)
{
	auto taskArg = static_cast<TaskParams*>(argument);
	auto imageCapturer = taskArg->imageCapturer;//std::make_shared<business_logic::DataHandling::ImageCapturer>(*static_cast<business_logic::DataHandling::ImageCapturer*>(taskArg->imageCapturer.get()));
	auto sharedClkMng  = taskArg->sharedClkMng;
	const auto periodTimeCaptureImage = 10000;
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
		LOG_TRACE("SystemTasks::edgeDetection captureImage");
		imageCapturer->captureImage();
		const auto captureTimestamp = sharedClkMng->getTimeReference().toNs();
		imageCapturer->extractImage();

		const uint8_t* rawImgBuffer = imageCapturer->getRawImageBuffer();
		size_t bufferSize = imageCapturer->getRawImageBufferSize();

		freeHeapSize = xPortGetFreeHeapSize();
		minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();

		auto edgesPtr = edges->data();
		const auto edgeCompressedImgSize = 3500;//imageCapturer->processEdges(rawImgBuffer, edgesPtr, bufferSize);
		static uint8_t captureId = 0;
		business_logic::DataSerializer::ImageSnapshot edgesSnapshot{captureId, 0x00, edgesPtr, edgeCompressedImgSize, captureTimestamp};
		captureId++;
		if(captureId == 4) captureId = 0;
		const auto isInserted = m_capturesQueue->sendToBack(( void * ) &edgesSnapshot);
		LOG_TRACE("SystemTasks::edgeDetection captureImage INSERTED to queue");
		if(!isInserted)
		{
			LOG_ERROR("Failed to insert snapshot");
		}
		LOG_TRACE("SystemTasks::edgeDetection captureImage done");
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
		const auto pendingMsgs = isPendingData();
		if(isPendingData())
		{
			LOG_INFO("Sending image information to master node. PendingMSg: ", std::to_string(pendingMsgs));
			business_logic::DataSerializer::ImageSnapshot nextSnapshot;
			getNextImage(nextSnapshot);
			LOG_INFO(" PendingMSg after getNextImage: ", isPendingData());
			for(size_t i = 0; i < (nextSnapshot.m_imgSize / MAXIMUN_CBOR_BUFFER_SIZE); i++)
		    {
				business_logic::DataSerializer::ImageSnapshot cborImgChunk{nextSnapshot.m_msgId, i, nextSnapshot.m_imgBuffer + (i*MAXIMUN_CBOR_BUFFER_SIZE), MAXIMUN_CBOR_BUFFER_SIZE, nextSnapshot.m_timestamp};
		    	const auto ptrImgChunkBuffer = cborImgChunk.m_imgBuffer;
				std::vector<uint8_t> cborSerializedChunk;
		        m_dataSerializer->serialize(cborImgChunk, cborSerializedChunk);
		        const auto ptrSerializedMsg = cborSerializedChunk.data();
		        const auto serializedMsgSize = cborSerializedChunk.size();

		        std::vector<business_logic::Communication::CanMsg> canMsgChunks;
		        const auto cborIndex = (nextSnapshot.m_msgId << 6) | (i & 0x3F);
		        splitCborToCanMsgs(cborIndex, cborSerializedChunk, canMsgChunks);
//				generateCanMsgsTest(cborIndex, cborSerializedChunk, canMsgChunks);
				commMng->sendData(canMsgChunks);
		    }
			LOG_INFO("Sending image information to master node done");
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
		const bool isTimeUpdated = sharedClkMng->getGlobalTime();
		if(isTimeUpdated)
		{
			const auto updatedTime = sharedClkMng->getTimeReference().toNs();
			LOG_INFO("SystemTasks::syncronizationGlobalClock Updated global master time: ", updatedTime);
		}
		m_clockSyncTaskHandler->delay(syncClkPeriod);
	}
	/* USER CODE END 5 */
}

uint8_t SystemTasks::isPendingData()
{
	const auto pendingMsg = m_capturesQueue->getStoredMsg();
	return pendingMsg;
}

void SystemTasks::getNextImage(business_logic::DataSerializer::ImageSnapshot& edgesSnapshot)
{
	LOG_INFO("SystemTasks::getNextImage");
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

        for (size_t j = startIdx; j < endIdx - 1; ++j)
        {
            canMsg.payload[j - startIdx] = cborSerializedChunk[j];
        }
        canMsgChunks.push_back(canMsg);
    }
}
}



