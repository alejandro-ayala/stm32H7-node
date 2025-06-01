#include "QueueHandler.h"
#include "services/Exception/SystemExceptions.h"

namespace business_logic
{
namespace Osal
{
QueueHandler::QueueHandler(uint32_t queuelength, uint32_t itemSize) : m_queueLength(queuelength), m_itemSize(itemSize)
{
	createQueue();
}

QueueHandler::~QueueHandler()
{
	deleteQueue();
}

void QueueHandler::createQueue()
{
	m_queue = xQueueCreate( m_queueLength, m_itemSize );
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::createQueue error");
    if( m_queue == NULL )
    {
        /* Queue was not created and must not be used. */
    }
}

void QueueHandler::deleteQueue()
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::deleteQueue error");

	vQueueDelete(m_queue);
}

void QueueHandler::resetQueue()
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::resetQueue error");

	xQueueReset(m_queue);
}

void QueueHandler::removeQueue()
{

}


const char* QueueHandler::getName() const
{
	return pcQueueGetName(m_queue);
}

bool QueueHandler::sendToBack(const std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& itemToQueue)
{
    auto itemPtr = new std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>(itemToQueue);
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler:sendToBack: error");

    return xQueueSendToBack(m_queue, &itemPtr, static_cast<TickType_t>(0)) == pdPASS;
}

bool QueueHandler::sendToBack(const void * itemToQueue, uint32_t timeout)
{
	auto status = true;
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::sendToBack error");

	if( xQueueSendToBack( m_queue, itemToQueue, static_cast<TickType_t>(timeout) ) != pdPASS )
	{
		status = false;
	}
	return status;
}

void QueueHandler::sendToBackOverwrite(const void * itemToQueue)
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::sendToBackOverwrite error");

	xQueueOverwrite(m_queue, itemToQueue);
}

void QueueHandler::sendToFront(const void * itemToQueue)
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::sendToFront1 error");

	xQueueSendToFront(m_queue, itemToQueue, static_cast<TickType_t>(0));
}

void QueueHandler::sendToFront(const void * itemToQueue, uint32_t timeout)
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::sendToFront error");

	xQueueSendToFront( m_queue, itemToQueue, static_cast<TickType_t>(timeout));
}

bool QueueHandler::receive(std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& rxBuffer)
{
    std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>* itemPtr = nullptr;
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::receive1 error");

    if (xQueueReceive(m_queue, &itemPtr, static_cast<TickType_t>(portMAX_DELAY)) == pdPASS)
    {
        // Transferimos la propiedad del shared_ptr
        rxBuffer = std::move(*itemPtr);
        delete itemPtr;  // Limpiamos el puntero crudo
        return true;
    }
    return false;
}

bool QueueHandler::receive(void *rxBuffer, uint32_t timeout)
{
	auto isReceived = false;
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::receive error");

	if(xQueueReceive( m_queue, rxBuffer,static_cast<TickType_t>(timeout) ) == pdPASS )
	{
		isReceived = true;
		/* xRxedStructure now contains a copy of xMessage. */
	}
	return isReceived;
}

bool QueueHandler::peek(void *rxBuffer)
{
	auto isReceived = false;
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::peek2 error");

	if( xQueuePeek( m_queue, rxBuffer, static_cast<TickType_t>(0)) )
	{
		isReceived = true;
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
	return isReceived;
}

bool QueueHandler::peek(void *rxBuffer, uint32_t timeout)
{
	auto isReceived = false;
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::peek error");

	if( xQueuePeek( m_queue, rxBuffer, static_cast<TickType_t>(timeout)) )
	{
		isReceived = true;
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
	return isReceived;
}

uint32_t QueueHandler::getStoredMsg() const
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::getStoredMsg error");

	return static_cast<uint32_t>(uxQueueMessagesWaiting(m_queue));
}

uint32_t QueueHandler::getAvailableSpace() const
{
	BUSINESS_LOGIC_ASSERT( m_queue != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "QueueHandler::getAvailableSpace error");
	return static_cast<uint32_t>(uxQueueSpacesAvailable(m_queue));
}
}
}
