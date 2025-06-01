#include "MutexHandler.h"
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"
namespace business_logic
{
namespace Osal
{
MutexHandler::MutexHandler() : timeout(1)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	BUSINESS_LOGIC_ASSERT( mutex != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "MutexHandler::MutexHandler error");
	 xSemaphoreGive(mutex);
}

MutexHandler::MutexHandler(uint32_t timeout) : timeout(timeout)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	BUSINESS_LOGIC_ASSERT( mutex != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "MutexHandler::MutexHandler(uint32_t timeout) error");
	 xSemaphoreGive(mutex);
}

MutexHandler::~MutexHandler()
{
	LOG_INFO("MutexHandler::~MutexHandler");
	vSemaphoreDelete(mutex);
}


void MutexHandler::lock()
{
	BUSINESS_LOGIC_ASSERT( mutex != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "MutexHandler::lock error");
	LOG_INFO("lock");
	xSemaphoreTake(mutex, portMAX_DELAY);
	LOG_INFO("lockED");
}

bool MutexHandler::try_lock()
{
	BUSINESS_LOGIC_ASSERT( mutex != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "MutexHandler::try_lock error");
	LOG_INFO("trylock");
	if(xSemaphoreTake(mutex, pdMS_TO_TICKS(timeout*1000)))
	{
		return true;
	}
	else
	{
		return false;
	}
	LOG_INFO("trylocked");
}

void MutexHandler::unlock()
{
	BUSINESS_LOGIC_ASSERT( mutex != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "MutexHandler::unlock error");
	LOG_INFO("unlock");
	xSemaphoreGive(mutex);
	LOG_INFO("locked");
}
}
}
