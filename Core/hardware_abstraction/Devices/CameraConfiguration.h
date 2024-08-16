#pragma once

namespace hardware_abstraction
{
namespace Devices
{

struct CameraConfiguration
{
	DCMI_HandleTypeDef hdcmi;
	DMA_HandleTypeDef hdma_dcmi;
	I2C_HandleTypeDef hi2c2;
	CameraResolution cameraResolution;
};

}
}
