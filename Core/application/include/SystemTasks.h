#pragma once
#include "business_logic/Osal/TaskHandler.h"
#include "business_logic/Communication/CommunicationManager.h"
#include "business_logic/ClockSyncronization/SharedClockSlaveManager.h"
#include "business_logic/DataHandling/ImageCapturer/ImageCapturer.h"
#include "main.h"
#include "cmsis_os.h"
#include <memory>

namespace application
{
constexpr uint32_t DefaultPriorityTask = 0;
class SystemTasks
{
private:

	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_clockSyncTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_dataHandlingTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_commTaskHandler;

public:
	SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng);
	virtual ~SystemTasks() = default;
	static void syncronizationGlobalClock(void* argument);
	static void captureImage(void* argument);
	static void sendData(void* argument);
    void createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng);
};

}
