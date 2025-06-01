#pragma once

#include <vector>
#include <utility>
#include <memory>
#include "../ICameraDevice.h"
#include  "hardware_abstraction/Controllers/I2C/I2CController.h"

static constexpr uint32_t maxBufferSize = 25000;
extern __attribute__((section(".RAM_D1"))) __attribute__((aligned(32))) uint8_t m_frameBuffer[maxBufferSize];

namespace hardware_abstraction
{
namespace Devices
{

class Ov2640Ctrl : public ICameraDevice
{
private:

	size_t m_frameBufferSize;
	const uint32_t m_resolutionSize = 15534;
	const uint8_t m_pid = 0x26;
	const uint8_t m_version = 0x42;

	static inline bool m_isCapturingFrame;

	DCMI_HandleTypeDef m_hdcmi;
	DMA_HandleTypeDef m_hdma_dcmi;
	I2C_HandleTypeDef m_hi2c2;

	CameraResolution m_imageResolution;
	std::shared_ptr<Controllers::I2CController> m_i2cControl;

	void setRegistersConfiguration(const std::vector<std::pair<uint8_t, uint8_t>>& registerCfg);
	uint32_t processCapture();

	short SCCB_Read(uint8_t reg_addr, uint8_t *pdata);
	short SCCB_Write(uint8_t reg_addr, uint8_t data);
public:
	Ov2640Ctrl(CameraConfiguration cfg);
	virtual ~Ov2640Ctrl() = default;

    void initialization() final;
    void initializationDCMI();
    void configuration(CameraResolution resolution) final;
	bool captureSnapshot() final;

	size_t getImageBufferSize() const final;
	const uint8_t* getImageBuffer() const final;
	CameraResolution getImageResolution() const final;

	void startContinuousCapture() final;
	void stopCapture() final;
	bool isCapturingFrame() const final;
	void decodeCapture();
	bool selfTest() final;
	void onFrameCaptured();
};
}
}
