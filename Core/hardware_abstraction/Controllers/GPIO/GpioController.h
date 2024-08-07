#pragma once

#include "stdio.h"
#include <stdint.h>
#include "../IController.h"

namespace hardware_abstraction
{
namespace Controllers
{
class GpioController : public IController
{
private:

	int deviceId;

public:
	GpioController();
	virtual ~GpioController();

	virtual void initialize() override;
	void    writePin(uint16_t port, uint16_t gpio, uint16_t value);
	uint8_t readPin(uint16_t port, uint16_t gpio);
	bool selfTest() override;
};
}
}
