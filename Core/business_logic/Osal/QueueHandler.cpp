#include "QueueHandler.h"

namespace business_logic
{
namespace Osal
{
QueueHandler::QueueHandler(uint32_t size, uint32_t length) : m_size(size), m_length(length)
{
	createQueue();
}

QueueHandler::~QueueHandler()
{
	deleteQueue();
}

void QueueHandler::createQueue()
{
	m_queue = xQueueCreate( m_size, m_length );

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

void QueueHandler::sendToBack(const void * itemToQueue)
{
	if( xQueueSendToBack( m_queue, itemToQueue, static_cast<TickType_t>(0) ) != pdPASS )
	{
		/* Failed to post the message, even after 10 ticks. */
	}
}

void QueueHandler::sendToBack(const void * itemToQueue, uint32_t timeout)
{
	if( xQueueSendToBack( m_queue, itemToQueue, static_cast<TickType_t>(timeout) ) != pdPASS )
	{
		/* Failed to post the message, even after 10 ticks. */
	}
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
	
void QueueHandler::receive(void *rxBuffer)
{
	if(xQueueReceive( m_queue, rxBuffer,static_cast<TickType_t>(portMAX_DELAY) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void QueueHandler::receive(void *rxBuffer, uint32_t timeout)
{
	if(xQueueReceive( m_queue, rxBuffer,static_cast<TickType_t>(timeout) ) == pdPASS )
	{
		/* xRxedStructure now contains a copy of xMessage. */
	}
}

void QueueHandler::peek(void *rxBuffer)
{
	if( xQueuePeek( m_queue, rxBuffer, static_cast<TickType_t>(0)) )
	{
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
}

void QueueHandler::peek(void *rxBuffer, uint32_t timeout)
{
	if( xQueuePeek( m_queue, rxBuffer, static_cast<TickType_t>(timeout)) )
	{
		// pcRxedMessage now points to the struct AMessage variable posted
		// by vATask, but the item still remains on the queue.
	}
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
