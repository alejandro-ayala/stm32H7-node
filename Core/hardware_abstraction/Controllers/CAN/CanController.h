#pragma once
#include "stm32h7xx_hal.h"
#include "business_logic/Communication/CanFrame.h"
#include "../IController.h"

namespace hardware_abstraction
{
namespace Controllers
{
class CanController : public IController
{
private:
	FDCAN_HandleTypeDef hfdcan1;
	FDCAN_FilterTypeDef sFilterConfig;
	FDCAN_TxHeaderTypeDef txHeader;
	FDCAN_RxHeaderTypeDef rxHeader;
	int deviceId;

public:
	CanController();
	virtual ~CanController();

	virtual void initialize() override;
	int transmitMsg(uint8_t idMsg, const uint8_t *txMsg, uint8_t dataSize);
	business_logic::Communication::CanFrame receiveMsg();
	bool selfTest() override;
	void integrationTest();
	void externalLoopbackTest();
};
}
}
