#pragma once
#include "stm32h7xx_hal.h"

#include "../IController.h"
#include "../ICommunication.h"

namespace hardware_abstraction
{
namespace Controllers
{
class UARTController : public ICommunication
{
private:
	UART_HandleTypeDef uartHandle;
	uint32_t blockingTime;

public:
	UARTController(const UART_HandleTypeDef* uartCfg);
	virtual ~UARTController() = default;

	void initialize()                     override;
	void send(const std::string& command) override;
	void receive(std::string& response, uint32_t waitResponse = 2000)   override;
	bool selfTest()                       override;
};
}
}
