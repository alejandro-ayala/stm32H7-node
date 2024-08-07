#pragma once

#include <FreeRTOS.h>
#include "semphr.h"

namespace business_logic
{
namespace Osal
{
class QueueHandler
{
private:
	QueueHandle_t m_queue;
	uint32_t m_size;
	uint32_t m_length;
	void createQueue();
	void deleteQueue();
public:

	QueueHandler(uint32_t size, uint32_t length);
	virtual ~QueueHandler();


	void resetQueue();
	void removeQueue();
	const char* getName() const;
	void sendToBack(const void * itemToQueue);
	void sendToBack(const void * itemToQueue, uint32_t timeout);	
	void sendToBackOverwrite(const void * itemToQueue);
	void sendToFront(const void * itemToQueue);
	void sendToFront(const void * itemToQueue, uint32_t timeout);

	void receive(void *rxBuffer);
	void receive(void *rxBuffer, uint32_t timeout);

	void peek(void *rxBuffer);
	void peek(void *rxBuffer, uint32_t timeout);
	uint32_t getStoredMsg() const;
	uint32_t getAvailableSpace() const;
};
}
}
