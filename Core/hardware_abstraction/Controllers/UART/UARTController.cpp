#include "../../Controllers/UART/UARTController.h"
#include "services/Exception/SystemExceptions.h"
#include <string.h>

namespace hardware_abstraction
{
namespace Controllers
{

UARTController::UARTController(const UART_HandleTypeDef* uartCfg) : ICommunication("UARTController"), uartHandle(*uartCfg), blockingTime(500)
{

}

void UARTController::initialize()
{
	HAL_UART_DeInit(&uartHandle);
	if(HAL_UART_Init(&uartHandle) != HAL_OK)
	{
		THROW_CONTROLLERS_EXCEPTION(services::UartInitializationError, "Uart controller initialization error")
	}
}

void UARTController::receive(std::string& response, uint32_t waitResponse)
{
	static const uint8_t atMaxResponseSize = 100;
	uint8_t buffer[atMaxResponseSize];
	memset(buffer, 0, sizeof(atMaxResponseSize));

	const auto state = HAL_UART_Receive(&uartHandle,(uint8_t*) buffer, atMaxResponseSize, waitResponse);
	if(state)
	{
		//THROW_CONTROLLERS_EXCEPTION(services::UartRxError, "Uart controller receiving error")
	}
	const auto rxBytes = atMaxResponseSize - uartHandle.RxXferCount;
	response.assign(reinterpret_cast<const char*>(buffer), rxBytes);
}

void UARTController::send(const std::string& command)
{
	uint32_t size = command.length();
	uint8_t* data = (uint8_t*)(command.c_str());
	data[size++] = '\r';
	data[size++] = '\n';
	data[size] = '\0';

	const auto state = HAL_UART_Transmit(&uartHandle, data, size, blockingTime);
	if(state)
	{
		THROW_CONTROLLERS_EXCEPTION(services::UartTxError, "Uart controller sending error")
	}
}

bool UARTController::selfTest()
{
	return true;
}
}
}
