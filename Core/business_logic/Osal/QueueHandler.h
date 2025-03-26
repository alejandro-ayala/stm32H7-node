#pragma once

#include <FreeRTOS.h>
#include "semphr.h"
#include <memory>
#include "business_logic/DataSerializer/ImageSnapshot.h"
namespace business_logic
{
namespace Osal
{
class QueueHandler
{
private:
	QueueHandle_t m_queue;
	uint32_t m_queueLength;
	uint32_t m_itemSize;

	void createQueue();
	void deleteQueue();
public:

	QueueHandler(uint32_t queuelength, uint32_t itemSize);
	virtual ~QueueHandler();


	void resetQueue();
	void removeQueue();
	const char* getName() const;
	bool sendToBack(const std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& itemToQueue);
	bool sendToBack(const void * itemToQueue, uint32_t timeout);
	void sendToBackOverwrite(const void * itemToQueue);
	void sendToFront(const void * itemToQueue);
	void sendToFront(const void * itemToQueue, uint32_t timeout);
	bool receive(std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& rxBuffer);
	bool receive(void *rxBuffer, uint32_t timeout);
	bool peek(void *rxBuffer);
	bool peek(void *rxBuffer, uint32_t timeout);
	uint32_t getStoredMsg() const;
	uint32_t getAvailableSpace() const;
};
}
}
