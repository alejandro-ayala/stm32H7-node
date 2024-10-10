#pragma once
#include "business_logic/Osal/TaskHandler.h"
#include "business_logic/Communication/CommunicationManager.h"
#include "business_logic/ClockSyncronization/SharedClockSlaveManager.h"
#include "business_logic/DataHandling/ImageCapturer/ImageCapturer.h"
#include "business_logic/DataSerializer/IDataSerializer.h"
#include "business_logic/Communication/CanFrame.h"

#include "main.h"
#include "cmsis_os.h"
#include <memory>
#include <vector>

namespace application
{
typedef struct
{
	std::shared_ptr<business_logic::DataHandling::ImageCapturer> imageCapturer;
	std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager> sharedClkMng;
} TaskParams;
constexpr uint32_t DefaultPriorityTask = 0;
class SystemTasks
{
private:

	TaskParams m_taskParam;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_clockSyncTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_dataHandlingTaskHandler;
	static inline std::shared_ptr<business_logic::Osal::TaskHandler> m_commTaskHandler;
	static inline std::shared_ptr<business_logic::DataSerializer::IDataSerializer> m_dataSerializer;
	static inline std::shared_ptr<business_logic::Osal::QueueHandler> m_capturesQueue;

public:
	SystemTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng);
	virtual ~SystemTasks() = default;
	static void syncronizationGlobalClock(void* argument);
	static void edgeDetection(void* argument);
	static void sendData(void* argument);
    void createPoolTasks(const std::shared_ptr<business_logic::Communication::CommunicationManager>& commMng, const std::shared_ptr<business_logic::DataHandling::ImageCapturer>& imageCapturer, const std::shared_ptr<business_logic::ClockSyncronization::SharedClockSlaveManager>& sharedClkMng);
    static void getNextImage(business_logic::DataSerializer::ImageSnapshot& edgesSnapshot);
    static bool isPendingData();
    static void splitCborToCanMsgs(uint8_t canMsgId, const std::vector<uint8_t>& cborSerializedChunk, std::vector<business_logic::Communication::CanMsg>& canMsgChunks);
};

}
