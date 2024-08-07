#include "MutexHandler.h"

namespace business_logic
{
namespace Osal
{
MutexHandler::MutexHandler() : timeout(1)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	 xSemaphoreGive(mutex);
}

MutexHandler::MutexHandler(uint32_t timeout) : timeout(timeout)
{
	mutex = xSemaphoreCreateBinary();//xSemaphoreCreateMutex();
	 xSemaphoreGive(mutex);
}

MutexHandler::~MutexHandler()
{
	vSemaphoreDelete(mutex);
}


void MutexHandler::lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreTake(mutex, portMAX_DELAY);
}

bool MutexHandler::try_lock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	if(xSemaphoreTake(mutex, pdMS_TO_TICKS(timeout*1000)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MutexHandler::unlock()
{
	//ASSERT_FUNC(ERROR_CODE::NULL_PARAMETER, mutex != nullptr, "SPD mutex is null");
	xSemaphoreGive(mutex);
}
}
}
