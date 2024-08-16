#pragma once
#include "hardware_abstraction/Devices/CameraResolution.h"
#include "services/Exception/SystemExceptions.h"
#include <cstdint>

namespace business_logic
{
namespace DataHandling
{
struct ImageConfiguration
{
	uint32_t imageWidth;
	uint32_t imageHeight;
	uint32_t imageQuality;
	ImageConfiguration() = default;

	ImageConfiguration(hardware_abstraction::Devices::CameraResolution cameraRes)
	{
		BUSINESS_LOGIC_ASSERT(services::BusinessLogicErrorId::InvalidArgument, cameraRes < hardware_abstraction::Devices::CameraResolution::RES_160X120 || cameraRes > hardware_abstraction::Devices::CameraResolution::RES_1280x960, "Invalid camera resolution. Can not be converted to ImageConfiguration");
		imageQuality = 25;
		switch (cameraRes) {
			case hardware_abstraction::Devices::CameraResolution::RES_160X120:
				imageWidth  = 160;
				imageHeight = 120;
				break;
			case hardware_abstraction::Devices::CameraResolution::RES_320X240:
				imageWidth  = 320;
				imageHeight = 240;
				break;
			case hardware_abstraction::Devices::CameraResolution::RES_640X480:
				imageWidth  = 640;
				imageHeight = 480;
				break;
			case hardware_abstraction::Devices::CameraResolution::RES_800x600:
				imageWidth  = 800;
				imageHeight = 600;
				break;
			case hardware_abstraction::Devices::CameraResolution::RES_1024x768:
				imageWidth  = 1024;
				imageHeight = 768;
				break;
			case hardware_abstraction::Devices::CameraResolution::RES_1280x960:
				imageWidth  = 1280;
				imageHeight = 960;
			default:
				break;
		}
	}

	operator uint32_t() const
	{
		return imageHeight * imageWidth;
	}
};

}
}
