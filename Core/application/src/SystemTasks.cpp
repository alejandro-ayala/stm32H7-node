#include "../include/SystemTasks.h"

#include "business_logic/DataSerializer/DataSerializer.h"

#include "services/Logger/LoggerMacros.h"

namespace application
{
extern "C" {
TIM_HandleTypeDef timStats;
#define MAX_REGISTRIES 10
typedef struct {
    const char *taskName;
    uint32_t totalTime;
    uint32_t minTime;
    uint32_t maxTime;
    uint32_t totalExecutions;
} RunTimeStats_t;

static RunTimeStats_t runtimeStatsRegistries[MAX_REGISTRIES];
static uint8_t currentRegisteredStats = 0;



#include <string.h>

extern uint32_t ulGetRunTimeCounterValue(void);

void RunTimeStats_Start(uint32_t *timestamp)
{
    *timestamp = ulGetRunTimeCounterValue();
}

void RunTimeStats_End(const char *taskName, uint32_t timestamp_start, bool printSample)
{

    uint32_t duration = ulGetRunTimeCounterValue() - timestamp_start;
//#ifdef RuntimeStats
    RunTimeStats_t *currentTime = NULL;
    for (int i = 0; i < currentRegisteredStats; i++) {
        if (strcmp(runtimeStatsRegistries[i].taskName, taskName) == 0) {
        	currentTime = &runtimeStatsRegistries[i];
            break;
        }
    }

    // Create new instance
    if (currentTime == NULL && currentRegisteredStats < MAX_REGISTRIES) {
    	runtimeStatsRegistries[currentRegisteredStats].taskName = taskName;
    	runtimeStatsRegistries[currentRegisteredStats].totalTime = 0;
    	runtimeStatsRegistries[currentRegisteredStats].minTime = 0xFFFFFFFF;
    	runtimeStatsRegistries[currentRegisteredStats].maxTime = 0;
    	runtimeStatsRegistries[currentRegisteredStats].totalExecutions = 0;
        currentTime = &runtimeStatsRegistries[currentRegisteredStats++];
    }

    if (currentTime) {
        currentTime->totalTime += duration;
        if (duration < currentTime->minTime) currentTime->minTime = duration;
        if (duration > currentTime->maxTime) currentTime->maxTime = duration;
        currentTime->totalExecutions++;
    }
//#endif
    if(printSample)
    {
        const std::string strRuntime = std::string(taskName) + " executed in " + std::to_string(duration) + " us";
        LOG_INFO(strRuntime);
    }
}
#include <sstream>
#include <string>

std::string RunTimeStats_FormatLog(const RunTimeStats_t& r)
{
    std::ostringstream oss;
    uint32_t averageTime = (r.totalExecutions > 0) ? (r.totalTime / r.totalExecutions) : 0;

    oss << r.taskName << ": executions=" << r.totalExecutions
        << ", min=" << r.minTime << " us"
        << ", max=" << r.maxTime << " us"
        << ", prom=" << averageTime << " us";

    return oss.str();
}


void RunTimeStats_Print(void)
{

    LOG_INFO("Runtime execution time");
    for (int i = 0; i < currentRegisteredStats; i++) {
    	std::string logEntry = RunTimeStats_FormatLog(runtimeStatsRegistries[i]);
    	LOG_INFO(logEntry);
    }
}


void vConfigureTimerForRunTimeStats(void)
{

	  /* USER CODE BEGIN TIM2_Init 0 */

	  /* USER CODE END TIM2_Init 0 */

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  /* USER CODE BEGIN TIM2_Init 1 */

	  /* USER CODE END TIM2_Init 1 */
	  timStats.Instance = TIM2;
	  timStats.Init.Prescaler = 274;
	  timStats.Init.CounterMode = TIM_COUNTERMODE_UP;
	  timStats.Init.Period = 0xFFFFFFFF ;
	  timStats.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  timStats.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(&timStats) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&timStats, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&timStats, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }

    HAL_TIM_Base_Start(&timStats);
}

uint32_t ulGetRunTimeCounterValue(void)
{
    return __HAL_TIM_GET_COUNTER(&timStats);
}
}

volatile bool transmisionOnGoing = false;
volatile bool clockSynq = false;

SystemTasks::SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	uint32_t queueItemSize   = sizeof(std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>);

	uint32_t queueLength     = 5;
	m_capturesQueue = std::make_shared<business_logic::Osal::QueueHandler>(queueLength, queueItemSize);
	createPoolTasks(commMng, imageCapturer, sharedClkMng);
	m_dataSerializer = std::make_shared<business_logic::DataSerializer::DataSerializer>();

	m_taskParam.imageCapturer = imageCapturer;
	m_taskParam.sharedClkMng  = sharedClkMng;

	LOG_TRACE(" Initial value of queue: ", std::to_string(isPendingData()));

	vConfigureTimerForRunTimeStats();
}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	try {
		m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()), 2048);
		m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::edgeDetection, "edgeDetection", DefaultPriorityTask + 1, static_cast<business_logic::Osal::VoidPtr>(&m_taskParam), 4096);
		m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask + 2, static_cast<business_logic::Osal::VoidPtr>(commMng.get()), 2048);

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
	const auto periodTimeCaptureImage = 8000;
	const auto delayCameraStartup     = 1000;


	imageCapturer->initialize();
	m_dataHandlingTaskHandler->delayUntil(delayCameraStartup);
	const auto imgBufferSize = imageCapturer->getBufferSize();
#ifdef EDGE_DETECTION
	auto edges   = std::make_shared<std::vector<uint8_t>>(imgBufferSize);
#endif
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::edgeDetection started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize) + " stackSize: " + std::to_string(uxTaskGetStackHighWaterMark( NULL ));
    LOG_TRACE(startMsg);

    uint32_t startExecutionTime;

	for(;;)
	{
		RunTimeStats_Start(&startExecutionTime);
		const auto t1 = xTaskGetTickCount();
		if(clockSynq && transmisionOnGoing == false)
		{
			transmisionOnGoing = true;
			logMemoryUsage();
			const auto isValidImg = imageCapturer->captureImage();
			if(isValidImg)
			{
				logMemoryUsage();
				const auto captureTimestamp = sharedClkMng->getLocalTimeReference();

				std::shared_ptr<business_logic::DataSerializer::ImageSnapshot> imageSnapshot;
				static uint8_t captureId = 0;
#ifdef EDGE_DETECTION
				imageCapturer->extractImage();
				const uint8_t* rawImgBuffer = imageCapturer->getRawImageBuffer();
				size_t bufferSize = imageCapturer->getRawImageBufferSize();

				freeHeapSize = xPortGetFreeHeapSize();
				minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();

				auto edgesPtr = edges->data();
				const auto edgeCompressedImgSize = imageCapturer->processEdges(rawImgBuffer, edgesPtr, bufferSize);
				auto edgesBuffer = std::shared_ptr<uint8_t[]>(new uint8_t[edgeCompressedImgSize]);
				std::memcpy(edgesBuffer.get(), edgesPtr, edgeCompressedImgSize);
				imageSnapshot = std::make_shared<business_logic::DataSerializer::ImageSnapshot>(
					captureId, 0x00, edgesBuffer, edgeCompressedImgSize, captureTimestamp
				);
#else
				const auto imgSize = imageCapturer->getJpegImageBufferSize();
				auto imgBuffer = imageCapturer->getJpegImageBuffer();
				auto imgBufferPtr = std::shared_ptr<uint8_t[]>(new uint8_t[imgSize]);
				std::memcpy(imgBufferPtr.get(), imgBuffer, imgSize);

				imageSnapshot = std::make_shared<business_logic::DataSerializer::ImageSnapshot>(
					captureId, 0x00, imgBufferPtr, imgSize, captureTimestamp
				);
#endif
				captureId++;
				if(captureId == 255) captureId = 0;
//				auto ptrImg = imageSnapshot->m_imgBuffer.get();
				const auto isInserted = m_capturesQueue->sendToBack(imageSnapshot);
				if(!isInserted)
				{
					LOG_ERROR("Failed to insert snapshot");
				}
				else
				{
					std::string strMsg = "SystemTasks::edgeDetection captureImage INSERTED to queue:";
					LOG_TRACE(strMsg);
				}

				logMemoryUsage();
				const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
				LOG_DEBUG("SystemTasks::edgeDetection captureImage executed at ", captureTimestamp, " -- executionTime:", std::to_string(executionTime));
			}
			else
			{
				transmisionOnGoing = false;
				LOG_WARNING("SystemTasks::edgeDetection invalid captured image");
			}
			RunTimeStats_End("edgeDetection", startExecutionTime, true);
		}


		m_dataHandlingTaskHandler->delay(periodTimeCaptureImage);
	}
	/* USER CODE END 5 */
}

void SystemTasks::sendData(void* argument)
{
	auto commMng = std::make_shared<business_logic::Communication::CommunicationManager>(*static_cast<business_logic::Communication::CommunicationManager*>(argument));

    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::sendData started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize) + " stackSize: " + std::to_string(uxTaskGetStackHighWaterMark( NULL ));
    LOG_INFO(startMsg);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */

    auto nextSnapshot = std::make_shared<business_logic::DataSerializer::ImageSnapshot>();
    uint32_t startExecutionTime;
	for(;;)
	{
		RunTimeStats_Start(&startExecutionTime);

		const auto pendingMsgs = getNextImage(nextSnapshot);

		if(pendingMsgs)
		{
			const auto t1 = xTaskGetTickCount();
			logMemoryUsage();


			const auto ptr = nextSnapshot->m_imgBuffer.get();
			LOG_INFO("Sending image information to master node. PendingMSg: ", std::to_string(pendingMsgs));
			//i++;
			for (size_t idxCbor = 0; idxCbor < nextSnapshot->m_imgSize; idxCbor += MAXIMUN_CBOR_BUFFER_SIZE)
			{
				size_t chunkSize = std::min(static_cast<size_t>(MAXIMUN_CBOR_BUFFER_SIZE), static_cast<size_t>(nextSnapshot->m_imgSize - idxCbor));

			    uint8_t msgIndex = static_cast<uint8_t>(idxCbor / MAXIMUN_CBOR_BUFFER_SIZE);

				auto chunkBuffer = std::shared_ptr<uint8_t[]>(new uint8_t[chunkSize]);
				std::memcpy(chunkBuffer.get(), nextSnapshot->m_imgBuffer.get() + idxCbor, chunkSize);
			    business_logic::DataSerializer::ImageSnapshot chunkSnapshot(
			        nextSnapshot->m_msgId,
			        msgIndex,
			        chunkBuffer,
					chunkSize,
			        nextSnapshot->m_timestamp
			    );

			    std::vector<uint8_t> cborSerializedChunk;
			    m_dataSerializer->serializeMsg(chunkSnapshot, cborSerializedChunk);

		        std::vector<business_logic::Communication::CanMsg> canMsgChunks;
		        const auto cborIndex = (nextSnapshot->m_msgId << 6) | (msgIndex & 0x3F);
		        splitCborToCanMsgs(cborIndex, cborSerializedChunk, canMsgChunks);
//				generateCanMsgsTest(cborIndex, cborSerializedChunk, canMsgChunks);
		        bool isLastIteration = (idxCbor + MAXIMUN_CBOR_BUFFER_SIZE >= nextSnapshot->m_imgSize);
				commMng->sendData(canMsgChunks, isLastIteration);
				bool receivedConfirmation = false;
				bool warningMisingAck = false;
				uint16_t confirmationTimeout = 0;
				while(!receivedConfirmation)
				{
					receivedConfirmation = commMng->waitingForConfirmation();
					if(!receivedConfirmation)m_commTaskHandler->delay(10);
					if( !warningMisingAck && confirmationTimeout > 500)
					{
						warningMisingAck = true;
						LOG_WARNING("SystemTasks::sendData missing receivedConfirmation for: ", std::to_string(nextSnapshot->m_msgId), "--", std::to_string(msgIndex), " -- size: ", std::to_string(canMsgChunks.size()));
						auto& lastMsg = canMsgChunks.back();
						LOG_WARNING(std::to_string(lastMsg.payloadSize), "--", std::to_string(lastMsg.payload[0])," ", std::to_string(lastMsg.payload[1]), " " ,std::to_string(lastMsg.payload[2]) ," " ,std::to_string(lastMsg.payload[3]) ,std::to_string(lastMsg.payload[4]) ," " ,std::to_string(lastMsg.payload[5]));

						receivedConfirmation = true;
					}
					confirmationTimeout++;
				}
				//LOG_INFO("CommunicationManager::waitingForConfirmation received FRAME_CONFIRMATION for: ", std::to_string(nextSnapshot->m_msgId), "--", std::to_string(msgIndex));

			}
			const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
			logMemoryUsage();
			RunTimeStats_End("sendData", startExecutionTime, true);
			LOG_INFO("SystemTasks::sendData executed in: ", std::to_string(executionTime), " ms");
			transmisionOnGoing = false;
		}
	}
	/* USER CODE END 5 */
}

void SystemTasks::syncronizationGlobalClock(void* argument)
{
	auto sharedClkMng = std::make_shared<business_logic::ClockSyncronization::SharedClockSlaveManager>(*static_cast<business_logic::ClockSyncronization::SharedClockSlaveManager*>(argument));
	sharedClkMng->initialization();

	const auto syncClkPeriod = 1000;
    size_t freeHeapSize = xPortGetFreeHeapSize();
    size_t minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
    const std::string startMsg = "SystemTasks::syncronizationGlobalClock started --> freeHeapSize: " + std::to_string(freeHeapSize) + " minEverFreeHeapSize " + std::to_string(minEverFreeHeapSize) + " stackSize: " + std::to_string(uxTaskGetStackHighWaterMark( NULL ));
    LOG_INFO(startMsg);
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
    uint32_t startExecutionTime;
    int32_t triggerSystemStats = 0;
	for(;;)
	{
		RunTimeStats_Start(&startExecutionTime);
		static int i = 0;
		const auto t1 = xTaskGetTickCount();
		//LOG_INFO("SystemTasks::syncronizationGlobalClock");
		if((i%10)  == 0)logMemoryUsage();
		const bool isTimeUpdated = sharedClkMng->synqGlobalTime();
		if(isTimeUpdated)
		{
			const auto updatedTime = sharedClkMng->getTimeReference().toNs();
			const auto localTime   = sharedClkMng->getLocalTimeReference();
			const auto executionTime = (xTaskGetTickCount() - t1) * portTICK_PERIOD_MS;
			const std::string logMsg = "SystemTasks::syncronizationGlobalClock Updated global master time to: " + std::to_string(updatedTime) + " previous(" + std::to_string(localTime) + "ms) executed in " + std::to_string(executionTime) + " ms";
			LOG_INFO(logMsg);
			clockSynq = true;
			if((i%10)  == 0){logMemoryUsage();i=0;}
		}
		i++;
		RunTimeStats_End("syncronizationGlobalClock", startExecutionTime, isTimeUpdated);
		m_clockSyncTaskHandler->delay(syncClkPeriod);

		if(triggerSystemStats > 10)
		{
//					LOG_INFO("*********FreeRTOS stats*********");
//					printSystemStats();
			LOG_INFO("*********System Custom stats*********");
			RunTimeStats_Print();
			LOG_INFO("***************************");
			triggerSystemStats = 0;
		}
		triggerSystemStats++;
	}
	/* USER CODE END 5 */
}

uint8_t SystemTasks::isPendingData()
{
	const auto pendingMsg = m_capturesQueue->getStoredMsg();
	return pendingMsg;
}

bool SystemTasks::getNextImage(std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& edgesSnapshot)
{
    LOG_TRACE("SystemTasks::getNextImage");
    return m_capturesQueue->receive(edgesSnapshot);
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



