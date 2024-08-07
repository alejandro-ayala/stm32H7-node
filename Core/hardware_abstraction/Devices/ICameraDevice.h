
#pragma once
#include "IDevice.h"
#include "../Controllers/Definitions.h"
#include "stm32h7xx_hal.h"

namespace hardware_abstraction
{
namespace Devices
{
struct CameraCfg
{
	DCMI_HandleTypeDef hdcmi;
	DMA_HandleTypeDef hdma_dcmi;
	I2C_HandleTypeDef hi2c2;
};

enum class CameraResolution
{
	RES_160X120,
	RES_320X240,
	RES_640X480,
	RES_800x600,
	RES_1024x768,
	RES_1280x960
};


class ICameraDevice : public IDevice
{
public:
	ICameraDevice() = default;
	~ICameraDevice() override = default;
	virtual void configuration(CameraResolution resolution) = 0;
	virtual void captureSnapshot() = 0;

	virtual size_t getImageBufferSize() const = 0;
	virtual const uint8_t* getImageBuffer() const = 0;
	virtual void startContinuousCapture() = 0;
	virtual void stopCapture() = 0;
	virtual bool isCapturingFrame() const = 0;
};
}
}
