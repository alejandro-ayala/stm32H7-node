#include "TaskHandler.h"
#include "services/Exception/SystemExceptions.h"

namespace business_logic
{
namespace Osal
{
TaskHandler::TaskHandler(TaskFunction taskFunction, const char* taskName)
{
	const auto created = xTaskCreate(taskFunction, taskName, StackSize, ( void * ) 1, tskIDLE_PRIORITY, &m_task);
	SERVICES_ASSERT( created == pdPASS, services::ServicesErrorId::RtosTaskCreationError, "Task: " + std::string(taskName) + " can not be created");
}

TaskHandler::TaskHandler(TaskFunction taskFunction, const char* taskName, uint32_t priority)
{
	const auto created = xTaskCreate(taskFunction, taskName, StackSize, ( void * ) 1, priority, &m_task);
	SERVICES_ASSERT( created == pdPASS, services::ServicesErrorId::RtosTaskCreationError, "Task: " + std::string(taskName) + " can not be created");
}

TaskHandler::TaskHandler(TaskFunction taskFunction, const char* taskName, uint32_t priority, VoidPtr taskParam, uint32_t stackSize)
{
	const auto created = xTaskCreate(taskFunction, taskName, stackSize, taskParam, priority, &m_task);
	SERVICES_ASSERT( created == pdPASS, services::ServicesErrorId::RtosTaskCreationError, "Task: " + std::string(taskName) + " can not be created");
}

TaskHandler::~TaskHandler()
{
	deleteMe();
}

void TaskHandler::deleteMe() const
{
	vTaskDelete(m_task);
}

void TaskHandler::suspend() const
{
	vTaskSuspend(m_task);
}


void TaskHandler::resume() const
{
	vTaskResume(m_task);
}

void TaskHandler::delay(const uint32_t milliSecond) const
{
	const TickType_t delay = milliSecond / portTICK_PERIOD_MS;
	vTaskDelay(delay);
}

void TaskHandler::delayUntil(const uint32_t milliSecond) const
{
	const TickType_t delay = milliSecond / portTICK_PERIOD_MS;
	static uint32_t previousWakeUp = xTaskGetTickCount ();

	vTaskDelayUntil( &previousWakeUp, delay );
}

uint32_t TaskHandler::getPriority() const
{
	const auto priority = uxTaskPriorityGet(m_task);
	return priority;
}

void TaskHandler::setPriority(uint32_t newPriority) const
{
	vTaskPrioritySet(m_task, newPriority);
}
}
}
