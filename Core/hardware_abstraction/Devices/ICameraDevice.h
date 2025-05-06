
#pragma once
#include "IDevice.h"
#include "CameraResolution.h"
#include "CameraConfiguration.h"

#include "../Controllers/Definitions.h"
#include "stm32h7xx_hal.h"

namespace hardware_abstraction
{
namespace Devices
{
class ICameraDevice : public IDevice
{
public:
	ICameraDevice() = default;
	~ICameraDevice() override = default;
	virtual void configuration(CameraResolution resolution) = 0;
	virtual void captureSnapshot() = 0;

	virtual size_t getImageSize() const = 0;
	virtual size_t getImageBufferSize() const = 0;
	virtual const uint8_t* getImageBuffer() const = 0;
	virtual void startContinuousCapture() = 0;
	virtual void stopCapture() = 0;
	virtual bool isCapturingFrame() const = 0;
	virtual CameraResolution getImageResolution() const = 0;
};
}
}
