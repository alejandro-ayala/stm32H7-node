#pragma once

#include <memory>
#include "../Controllers/I2C/I2CController.h"
namespace hardware_abstraction
{
namespace Devices
{

struct CameraConfiguration
{
	DCMI_HandleTypeDef& dcmiHandler;
	DMA_HandleTypeDef hdma_dcmi;
	std::shared_ptr<Controllers::I2CController> hi2c2;
	CameraResolution cameraResolution;
};

}
}
