#pragma once

#include <FreeRTOS.h>
#include "semphr.h"

namespace business_logic
{
namespace Osal
{
class MutexHandler
{
private:
	SemaphoreHandle_t  mutex;
public:
	uint32_t           timeout;

	MutexHandler();
	MutexHandler(uint32_t timeout);
	virtual ~MutexHandler();

	MutexHandler(const MutexHandler&) = delete;
	MutexHandler& operator=(const MutexHandler&) = delete;

	void lock();
	bool try_lock();
	void unlock();
};
}
}
