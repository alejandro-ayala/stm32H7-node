#pragma once
#include "stm32h7xx_hal.h"

#include "../IController.h"
#include "../ICommunication.h"

namespace hardware_abstraction
{
namespace Controllers
{
class I2CController
{
private:
	I2C_HandleTypeDef m_hi2c2;

public:
	I2CController(const I2C_HandleTypeDef* i2cCfg);
	virtual ~I2CController() = default;

	void initialize();
	short receive(uint8_t reg_addr, uint8_t *pdata);
	short send(uint8_t reg_addr, uint8_t data);
};
}
}
