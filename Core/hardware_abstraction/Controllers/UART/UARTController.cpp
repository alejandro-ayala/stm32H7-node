#include "../../Controllers/UART/UARTController.h"
#include "services/Exception/SystemExceptions.h"
#include <string.h>

namespace hardware_abstraction
{
namespace Controllers
{

UARTController::UARTController(const UART_HandleTypeDef* uartCfg) : ICommunication("UARTController"), uartHandle(*uartCfg), blockingTime(5000)
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

void UARTController::send(const char* data, size_t size)
{
    // Asegúrate de que data termina en '\0' si es necesario para tu hardware
    // Si quieres agregar \r\n, puedes hacerlo aquí:
    uint8_t buffer[512];
    size_t len = size;
    if (size + 2 < sizeof(buffer)) {
        memcpy(buffer, data, size);
        buffer[size++] = '\r';
        buffer[size++] = '\n';
        buffer[size] = '\0';
        len = size;
    } else {
        // Si el mensaje es muy largo, solo copia lo que cabe
        memcpy(buffer, data, sizeof(buffer) - 3);
        buffer[sizeof(buffer) - 3] = '\r';
        buffer[sizeof(buffer) - 2] = '\n';
        buffer[sizeof(buffer) - 1] = '\0';
        len = sizeof(buffer) - 1;
    }
    HAL_UART_Transmit(&uartHandle, buffer, len, blockingTime);
}

bool UARTController::selfTest()
{
	return true;
}
}
}
