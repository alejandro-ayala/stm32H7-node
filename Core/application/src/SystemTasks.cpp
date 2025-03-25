#include "../include/SystemTasks.h"

#include "business_logic/DataSerializer/DataSerializer.h"

#include "services/Logger/LoggerMacros.h"

namespace application
{
volatile bool transmisionOnGoing = false;
SystemTasks::SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	uint32_t queueItemSize   = sizeof(business_logic::DataSerializer::ImageSnapshot);
	uint32_t queueLength     = 10;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	createPoolTasks(commMng, imageCapturer, sharedClkMng);
	m_dataSerializer = std::make_shared<business_logic::DataSerializer::DataSerializer>();

	m_taskParam.imageCapturer = imageCapturer;
	m_taskParam.sharedClkMng  = sharedClkMng;

	LOG_TRACE(" Initial value of queue: ", std::to_string(isPendingData()));
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


	imageCapturer->initialize();
	m_dataHandlingTaskHandler->delayUntil(delayCameraStartup);
	const auto imgBufferSize = imageCapturer->getBufferSize();
	auto edges   = std::make_shared<std::vector<uint8_t>>(imgBufferSize);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::edgeDetection started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);
	for(;;)
	{
		if(transmisionOnGoing == false)
		{
			transmisionOnGoing = true;
			size_t freeHeapSize = xPortGetFreeHeapSize();
			size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
			const std::string freeHeapMsg = "SystemTasks::edgeDetection captureImage freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
			LOG_INFO(freeHeapMsg);
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
			auto ptrImg = edgesSnapshot.m_imgBuffer.get();
			const auto isInserted = m_capturesQueue->sendToBack(( void * ) &edgesSnapshot);
			LOG_TRACE("SystemTasks::edgeDetection captureImage INSERTED to queue");
			if(!isInserted)
			{
				LOG_ERROR("Failed to insert snapshot");
			}
			LOG_TRACE("SystemTasks::edgeDetection captureImage done");

		}
		m_dataHandlingTaskHandler->delay(periodTimeCaptureImage);
	}
	/* USER CODE END 5 */
}

void SystemTasks::sendData(void* argument)
{
	auto commMng = std::make_shared<business_logic::Communication::CommunicationManager>(*static_cast<business_logic::Communication::CommunicationManager*>(argument));

	const auto sendDataPeriod = 1000;
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::sendData started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */

	for(;;)
	{
		const auto pendingMsgs = isPendingData();
		if(isPendingData())
		{
		    size_t freeHeapSize = xPortGetFreeHeapSize();
		    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
		    const std::string freeHeapMsg = "SystemTasks::sendData freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
		    LOG_INFO(freeHeapMsg);
			LOG_DEBUG("Sending image information to master node. PendingMSg: ", std::to_string(pendingMsgs));
			business_logic::DataSerializer::ImageSnapshot nextSnapshot;
			getNextImage(nextSnapshot);
			auto ptrImgDeque = nextSnapshot.m_imgBuffer.get();
			LOG_TRACE(" PendingMSg after getNextImage: ", isPendingData());
			for (size_t i = 0; i < nextSnapshot.m_imgSize; i += MAXIMUN_CBOR_BUFFER_SIZE) {
			    // Calculamos el tamaño del chunk (para el último fragmento puede ser menor a 128 bytes)
				size_t chunkSize = std::min(static_cast<size_t>(MAXIMUN_CBOR_BUFFER_SIZE), static_cast<size_t>(nextSnapshot.m_imgSize - i));

			    // Copiar el chunk en un buffer
			    std::vector<uint8_t> destinationBuffer(chunkSize);
			    std::memcpy(destinationBuffer.data(), nextSnapshot.m_imgBuffer.get() + i, chunkSize);

/*
			    // Log para verificar el contenido del chunk (opcional)
			    std::ostringstream chunkLog;
			    chunkLog << "Chunk [" << (i / MAXIMUN_CBOR_BUFFER_SIZE) << "]: ";
			    for (size_t j = 0; j < chunkSize; j++) {
			        chunkLog << std::to_string(destinationBuffer[j]) << " ";
			    }
			    LOG_INFO(chunkLog.str());
*/
			    //Crear estructura a serializar
			    uint8_t msgIndex = static_cast<uint8_t>(i / MAXIMUN_CBOR_BUFFER_SIZE);
			    auto chunkBuffer = std::make_unique<uint8_t[]>(chunkSize);
			    std::memcpy(chunkBuffer.get(), destinationBuffer.data(), chunkSize);
			    business_logic::DataSerializer::ImageSnapshot chunkSnapshot(
			    	nextSnapshot.m_msgId,
			        msgIndex,
			        chunkBuffer.get(),
			        chunkSize,
					nextSnapshot.m_timestamp
			    );

			    std::ostringstream chunkLog;
			    chunkLog << "Chunk [" << static_cast<int>(msgIndex) << "]: ";
			    for (size_t j = 0; j < chunkSize; j++) {
			        chunkLog << std::to_string(destinationBuffer[j]) << " ";
			    }
			    LOG_INFO(chunkLog.str());

			    //Serializar el buffer de 128bytes
			    std::vector<uint8_t> cborSerializedChunk;
			    m_dataSerializer->serializeMsg(chunkSnapshot, cborSerializedChunk);


			}

			LOG_DEBUG("Sending image information to master node done");
			//transmisionOnGoing = false;
		}
		m_commTaskHandler->delayUntil(sendDataPeriod);
	}
	/* USER CODE END 5 */
}

void SystemTasks::syncronizationGlobalClock(void* argument)
{
	auto sharedClkMng = std::make_shared<business_logic::ClockSyncronization::SharedClockSlaveManager>(*static_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument));

	const auto syncClkPeriod = 15000;
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::syncronizationGlobalClock started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize);
    LOG_INFO(startMsg);
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
	LOG_TRACE("SystemTasks::getNextImage");
	m_capturesQueue->receive(&edgesSnapshot);
}

void SystemTasks::splitCborToCanMsgs(uint8_t canMsgId, const std::vector<uint8_t>& cborSerializedChunk, std::vector<business_logic::Communication::CanMsg>& canMsgChunks)
{
    size_t totalBytes = cborSerializedChunk.size();
    size_t payloadSize = MAXIMUM_CAN_MSG_SIZE - CAN_MSG_ID_FIELD_SIZE;
    size_t numberOfMsgs = (totalBytes + payloadSize - 1) / payloadSize;

    for (size_t i = 0; i < numberOfMsgs; ++i)
    {
        business_logic::Communication::CanMsg canMsg;

        canMsg.canMsgId =canMsgId;
        canMsg.canMsgIndex = static_cast<uint8_t>(i);
        size_t startIdx = i * payloadSize;
        size_t endIdx = std::min(startIdx + payloadSize -1, totalBytes - 1);

        canMsg.payloadSize = endIdx - startIdx + 1;

		std::string cborStr = "splitCborToCanMsgs: ";

        for (size_t j = startIdx; j <= endIdx; ++j)
        {
            canMsg.payload[j - startIdx] = cborSerializedChunk[j];
            cborStr += std::to_string(canMsg.payload[j - startIdx]) + " ";
        }
        //LOG_TRACE(cborStr);
        canMsgChunks.push_back(canMsg);
    }
}
}



