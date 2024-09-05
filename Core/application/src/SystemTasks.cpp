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
}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()));
	m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::captureImage, "readSensorsTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(imageCapturer.get()), 4096);
	m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(commMng.get()), 4096);

}

void SystemTasks::captureImage(void* argument)
{
	auto imageCapturer = std::make_shared<business_logic::DataHandling::ImageCapturer>(*static_cast<business_logic::DataHandling::ImageCapturer*>(argument));

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
		m_dataHandlingTaskHandler->delayUntil(periodTimeCaptureImage);
		imageCapturer->extractImage();

		const uint8_t* rawImgBuffer = imageCapturer->getRawImageBuffer();
		size_t bufferSize = imageCapturer->getRawImageBufferSize();

		freeHeapSize = xPortGetFreeHeapSize();
		minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();

		auto edgesPtr = edges->data();
		const auto edgeCompressedImgSize = imageCapturer->processEdges(rawImgBuffer, edgesPtr, bufferSize);

		business_logic::DataSerializer::ImageSnapshot edgesSnapshot{0xE3, 0x00, edgesPtr, edgeCompressedImgSize, 0xFE34};
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
				business_logic::DataSerializer::ImageSnapshot msg{nextSnapshot.m_msgId, i, nextSnapshot.m_imgBuffer + (i*MAXIMUN_CBOR_BUFFER_SIZE), MAXIMUN_CBOR_BUFFER_SIZE, nextSnapshot.m_timestamp};
		    	std::vector<uint8_t> serializeMsg;
		        m_dataSerializer->serialize(msg, serializeMsg);
		        const auto ptrSerializedMsg = serializeMsg.data();
		        const auto serializedMsgSize = serializeMsg.size();
				commMng->sendData(serializeMsg);
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
}
