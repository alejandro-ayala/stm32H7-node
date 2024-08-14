#pragma once

#include <vector>
#include <utility>
#include "../ICameraDevice.h"
namespace hardware_abstraction
{
namespace Devices
{
constexpr uint32_t maxBufferSize = 10500;

class Ov2640Ctrl : public ICameraDevice
{
private:

	uint8_t m_frameBuffer[maxBufferSize] = { 0 };
	size_t m_frameBufferSize;
	const uint32_t m_resolutionSize = 15534;
	const uint8_t m_pid = 0x26;
	const uint8_t m_version = 0x42;

	static inline bool m_isCapturingFrame;

	DCMI_HandleTypeDef m_hdcmi;
	DMA_HandleTypeDef m_hdma_dcmi;
	I2C_HandleTypeDef m_hi2c2;

	void setRegistersConfiguration(const std::vector<std::pair<uint8_t, uint8_t>>& registerCfg);
	uint32_t processCapture();

	short SCCB_Read(uint8_t reg_addr, uint8_t *pdata);
	short SCCB_Write(uint8_t reg_addr, uint8_t data);
public:
	Ov2640Ctrl(CameraCfg cfg);
	virtual ~Ov2640Ctrl() = default;

    void initialization() final;
    void configuration(CameraResolution resolution) final;
	void captureSnapshot() final;

	size_t getImageBufferSize() const final;
	const uint8_t* getImageBuffer() const final;

	void startContinuousCapture() final;
	void stopCapture() final;
	bool isCapturingFrame() const final;
	void decodeCapture();
	bool selfTest() final;
};
}
}
