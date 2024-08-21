#include "../include/SystemTasks.h"
#include <iostream>

namespace application
{
SystemTasks::SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	createPoolTasks(commMng, imageCapturer, sharedClkMng);
}

void SystemTasks::createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng)
{
	m_clockSyncTaskHandler    = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::syncronizationGlobalClock, "syncronizationGlobalClockTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(sharedClkMng.get()));
	m_dataHandlingTaskHandler = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::captureImage, "readSensorsTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(imageCapturer.get()), 4096);
	m_commTaskHandler         = std::make_shared<business_logic::Osal::TaskHandler>(SystemTasks::sendData, "sendDataTask", DefaultPriorityTask, static_cast<business_logic::Osal::VoidPtr>(commMng.get()));
}

void SystemTasks::captureImage(void* argument)
{
	auto imageCapturer = std::make_shared<business_logic::DataHandling::ImageCapturer>(*static_cast<business_logic::DataHandling::ImageCapturer*>(argument));

	const auto periodTimeCaptureImage = 2000;
	imageCapturer->initialize();
	const auto imgBufferSize = imageCapturer->getBufferSize();
	auto edges   = new uint8_t[imgBufferSize];
	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
	  imageCapturer->captureImage();
	  m_dataHandlingTaskHandler->delayUntil(periodTimeCaptureImage);
	  imageCapturer->extractImage();

      const uint8_t* rawImgBuffer = imageCapturer->getRawImageBuffer();
      size_t bufferSize = imageCapturer->getRawImageBufferSize();


      imageCapturer->processEdges(rawImgBuffer, edges, bufferSize);

//      const auto encodedImg = imageCapturer->encodeEdgesImage(const_cast<uint8_t*>(edges), bufferSize);
//      const auto encodedImgSize = encodedImg.size();
//      for(const auto& element : encodedImg)
//      {
//    	  std::cout << "element: " << element << std::endl;
//
//      }
	}

	delete[] edges;
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
}
