#include "QueueHandler.h"
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

    if( m_queue == NULL )
    {
        /* Queue was not created and must not be used. */
    }
}

void QueueHandler::deleteQueue()
{
	vQueueDelete(m_queue);
}

void QueueHandler::resetQueue()
{
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
    return xQueueSendToBack(m_queue, &itemPtr, static_cast<TickType_t>(0)) == pdPASS;
}

bool QueueHandler::sendToBack(const void * itemToQueue, uint32_t timeout)
{
	auto status = true;
	if( xQueueSendToBack( m_queue, itemToQueue, static_cast<TickType_t>(timeout) ) != pdPASS )
	{
		status = false;
	}
	return status;
}

void QueueHandler::sendToBackOverwrite(const void * itemToQueue)
{
	xQueueOverwrite(m_queue, itemToQueue);
}

void QueueHandler::sendToFront(const void * itemToQueue)
{
	xQueueSendToFront(m_queue, itemToQueue, static_cast<TickType_t>(0));
}

void QueueHandler::sendToFront(const void * itemToQueue, uint32_t timeout)
{
	xQueueSendToFront( m_queue, itemToQueue, static_cast<TickType_t>(timeout));
}

bool QueueHandler::receive(std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>& rxBuffer)
{
    std::shared_ptr<business_logic::DataSerializer::ImageSnapshot>* itemPtr = nullptr;

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
	return static_cast<uint32_t>(uxQueueMessagesWaiting(m_queue));
}

uint32_t QueueHandler::getAvailableSpace() const
{
	return static_cast<uint32_t>(uxQueueSpacesAvailable(m_queue));
}
}
}
