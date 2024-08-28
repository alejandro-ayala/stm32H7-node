#include "ov2640Config.h"
#include "Ov2640Ctrl.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "main.h"

namespace hardware_abstraction
{
namespace Devices
{

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	int a = 0;
	a+4;

	//cameraController->stopCapture();
}

Ov2640Ctrl::Ov2640Ctrl(CameraConfiguration cfg) : m_hdcmi(cfg.hdcmi), m_hdma_dcmi(cfg.hdma_dcmi), m_i2cControl(cfg.hi2c2), m_imageResolution(cfg.cameraResolution)
{
	m_isCapturingFrame = false;

}

void Ov2640Ctrl::initialization()
{
	m_i2cControl->initialize();
	HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	//TODO add register map instead of magic values
	m_i2cControl->send(0xff, 0x01);
	m_i2cControl->send(0x12, 0x80);
	HAL_Delay(1000);

	uint8_t pid;
	uint8_t ver;
	m_i2cControl->receive(0x0a, &pid);  // pid value is 0x26
	m_i2cControl->receive(0x0b, &ver);  // ver value is 0x42
	std::cout << "Camera PID:" << pid << " version: " << ver << std::endl;

	// Stop DCMI clear buffer
	stopCapture();
	HAL_Delay(1000); //TODO check if needed
}

void Ov2640Ctrl::setRegistersConfiguration(const std::vector<std::pair<uint8_t, uint8_t>>& registerCfg)
{
	for(const auto& [regAddr, regVal] : registerCfg)
	{
		m_i2cControl->send(regAddr, regVal);
		std::cout << "SCCB write register: " << std::to_string(regAddr) << std::to_string(regVal) << std::endl;
		HAL_Delay(10);
		uint8_t newRegVal;
		m_i2cControl->receive(regAddr, &newRegVal);
		if (regVal != newRegVal)
		{
			std::cout << "SCCB write failure: " << std::to_string(regAddr) << std::to_string(regVal) << std::endl;
		}
	}
}

void Ov2640Ctrl::configuration(CameraResolution resolution)
{
	setRegistersConfiguration(OV2640_JPEG_INIT);
	setRegistersConfiguration(OV2640_YUV422);
	setRegistersConfiguration(OV2640_JPEG);
	HAL_Delay(100);
	m_i2cControl->send(0xff, 0x01);
	HAL_Delay(100);
	m_i2cControl->send(0x15, 0x00);

	setRegistersConfiguration(OV2640_320x240_JPEG);
}

void Ov2640Ctrl::captureSnapshot()
{
	std::fill(std::begin(m_frameBuffer), std::end(m_frameBuffer), 0);;
	HAL_DCMI_Start_DMA(&m_hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)m_frameBuffer, m_resolutionSize);
	//__HAL_DCMI_ENABLE_IT(&m_hdcmi, DCMI_IT_FRAME);
	HAL_Delay(6000);
	HAL_DCMI_Suspend(&m_hdcmi);
	HAL_DCMI_Stop(&m_hdcmi);


	const auto imgSize = processCapture();
	std::cout << "Image size:" << std::to_string(imgSize) << " bytes" << std::endl;
}

uint32_t Ov2640Ctrl::processCapture()
{
	uint32_t bufferPointer = 0;
	bool headerFound;
	m_frameBufferSize = 0;
	while (1)
	{
		if (!headerFound && m_frameBuffer[bufferPointer] == 0xFF
				&& m_frameBuffer[bufferPointer + 1] == 0xD8)
		{
			headerFound = true;
			std::cout << "Found header of JPEG file" << std::endl;
		}
		if (headerFound && m_frameBuffer[bufferPointer] == 0xFF
				&& m_frameBuffer[bufferPointer + 1] == 0xD9)
		{
			bufferPointer = bufferPointer + 2;
			std::cout << "Found EOI of JPEG file" << std::endl;
			headerFound = false;

			break;
		}
		if (bufferPointer >= maxBufferSize)
		{
			std::cout << "Excedded maximum buffer size" << std::endl;
			break;
		}
		bufferPointer++;
	}
	m_frameBufferSize = bufferPointer;
	std::cout << "Image size:" << std::to_string(bufferPointer) << " bytes" << std::endl;
    return bufferPointer;
}

bool Ov2640Ctrl::isCapturingFrame() const
{
	return m_isCapturingFrame;
}

const uint8_t* Ov2640Ctrl::getImageBuffer() const
{
    return m_frameBuffer;
}

size_t Ov2640Ctrl::getImageBufferSize() const
{
    return m_frameBufferSize;
}

CameraResolution Ov2640Ctrl::getImageResolution() const
{
    return m_imageResolution;
}

void Ov2640Ctrl::decodeCapture()
{

//	ov2640_decodeJPEG(m_frameBuffer, bufferPointer, 1); // Image decoding, grayscale selection.
//	HAL_Delay(300);
//	ov2640_encodeJPEG(&imageBuffer, &imageSize, 16); // Encoding image with quality set to 16%.
//	//HAL_UART_Transmit_DMA(&huart3, imageBuffer, imageSize); // Sending buffer to UART data output.
//	HAL_Delay(300);
}

void Ov2640Ctrl::startContinuousCapture()
{
	std::fill(std::begin(m_frameBuffer), std::end(m_frameBuffer), 0);
	m_isCapturingFrame = true;
	HAL_DCMI_Start_DMA(&m_hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)m_frameBuffer, m_resolutionSize);
}

void Ov2640Ctrl::stopCapture()
{
    HAL_DCMI_Suspend(&m_hdcmi);
    HAL_DCMI_Stop(&m_hdcmi);
    m_isCapturingFrame = false;
}

bool Ov2640Ctrl::selfTest()
{
	uint8_t pid;
	uint8_t version;
	m_i2cControl->receive(0x0a, &pid);  // pid value is 0x26
	m_i2cControl->receive(0x0b, &version);  // ver value is 0x42
	return (pid == m_pid && version == m_version);
}
}
}
