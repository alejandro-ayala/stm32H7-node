#pragma once
#include "stm32h7xx_hal.h"

#include "../IController.h"

namespace hardware_abstraction
{
namespace Controllers
{
class CanController : public IController
{
private:
	FDCAN_HandleTypeDef hfdcan1;

	uint32_t TxMailbox;
	int deviceId;

public:
	CanController();
	virtual ~CanController();

	virtual void initialize() override;
	int transmitMsg(uint8_t idMsg, uint8_t *TxMsg, uint8_t msgLength);
	int receiveMsg(uint8_t *rxBuffer);
	bool selfTest() override;
};
}
}
