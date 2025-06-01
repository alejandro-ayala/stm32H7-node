#include "ov2640Config.h"
#include "Ov2640Ctrl.h"
#include "services/Logger/LoggerMacros.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include "main.h"
#include "services/Exception/SystemExceptions.h"
DCMI_HandleTypeDef hdcmi;
__attribute__((section(".RAM_D1"))) __attribute__((aligned(32))) uint8_t m_frameBuffer[maxBufferSize];

namespace hardware_abstraction
{
namespace Devices
{
volatile bool frameCaptured;

void InvalidateDCache_by_Addr(void *addr, uint32_t size)
{
    if (addr == nullptr || size == 0) return;

    uint32_t addr32 = (uint32_t)addr;
    uint32_t start_addr = addr32 & ~((uint32_t)0x1F);
    uint32_t end_addr = (addr32 + size + 31) & ~((uint32_t)0x1F);
    uint32_t aligned_size = end_addr - start_addr;

    SCB_InvalidateDCache_by_Addr((void*)start_addr, aligned_size);
}

extern "C" void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
//	LOG_INFO("HAL_DCMI_FrameEventCallback frameCaptured");
	frameCaptured = true;
}

Ov2640Ctrl::Ov2640Ctrl(CameraConfiguration cfg) : m_hdcmi(cfg.dcmiHandler), m_hdma_dcmi(cfg.hdma_dcmi), m_i2cControl(cfg.hi2c2), m_imageResolution(cfg.cameraResolution)
{
	m_isCapturingFrame = false;
}

void Ov2640Ctrl::initialization()
{
	//CleanDCache_by_Addr((uint32_t*)m_frameBuffer, m_resolutionSize);
	initializationDCMI();
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
	LOG_INFO("Ov2640Ctrl::initialization: Camera PID:", std::to_string(pid), " VERSION: ", std::to_string(ver));

	// Stop DCMI clear buffer
	stopCapture();
	HAL_Delay(1000); //TODO check if needed
}

void Ov2640Ctrl::initializationDCMI(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
  hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.JPEGMode = DCMI_JPEG_ENABLE;
  hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
  hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
  hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
  hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  //HAL_DCMI_RegisterCallback(&hdcmi, HAL_DCMI_FRAME_EVENT_CB_ID, HAL_DCMI_FrameEventCallback);
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

void Ov2640Ctrl::setRegistersConfiguration(const std::vector<std::pair<uint8_t, uint8_t>>& registerCfg)
{
	for(const auto& [regAddr, regVal] : registerCfg)
	{
		m_i2cControl->send(regAddr, regVal);
		LOG_TRACE("Ov2640Ctrl::setRegistersConfiguration: Register:", regAddr, " = ", regVal);
		HAL_Delay(10);
		uint8_t newRegVal;
		m_i2cControl->receive(regAddr, &newRegVal);
		if (regVal != newRegVal)
		{
			LOG_DEBUG("Ov2640Ctrl::setRegistersConfiguration: write failure Register:", regAddr, " = ", regVal);
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

bool Ov2640Ctrl::captureSnapshot()
{
    std::fill(std::begin(m_frameBuffer), std::end(m_frameBuffer), 0);
    frameCaptured = false;
    if ((hdcmi.Instance->CR & DCMI_CR_ENABLE) != 0)
    {
        LOG_ERROR("DCMI still active!");
        HAL_DCMI_Stop(&hdcmi);
    }

    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)m_frameBuffer, maxBufferSize / 4);
    __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);

    uint32_t startTick = HAL_GetTick();
    while (!frameCaptured)
    {
        if ((HAL_GetTick() - startTick) > 2000)
        {
            LOG_ERROR("Timeout waiting for frame capture");
            HAL_DCMI_Stop(&hdcmi);
            return false;
        }
    }
    LOG_INFO("HAL_DCMI_FrameEventCallback frameCaptured");
//    DEVICES_ASSERT((uint32_t)m_frameBuffer & 0x1F, services::DevicesErrorId::Ov2460BufferNotAligned, "Buffer not aligned");
//    InvalidateDCache_by_Addr(m_frameBuffer, maxBufferSize);
    HAL_DCMI_Stop(&hdcmi);

    const auto imgSize = processCapture();
    LOG_INFO("Image size:", std::to_string(imgSize), " bytes");
    if(imgSize >= (maxBufferSize - 1))
        return false;
    else
        return true;
}

uint32_t Ov2640Ctrl::processCapture()
{
    uint32_t bufferPointer = 0;
    bool headerFound = false;
    m_frameBufferSize = 0;
    while (bufferPointer < maxBufferSize - 1)
    {
        if (!headerFound && m_frameBuffer[bufferPointer] == 0xFF
                && m_frameBuffer[bufferPointer + 1] == 0xD8)
        {
            headerFound = true;
            LOG_TRACE("Found header of JPEG file");
        }
        if (headerFound && m_frameBuffer[bufferPointer] == 0xFF
                && m_frameBuffer[bufferPointer + 1] == 0xD9)
        {
            bufferPointer += 2;
            LOG_DEBUG("Found EOI of JPEG file");
            break;
        }
        bufferPointer++;
    }
    m_frameBufferSize = bufferPointer;
    return m_frameBufferSize;
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
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)m_frameBuffer, m_resolutionSize);
}

void Ov2640Ctrl::stopCapture()
{
    HAL_DCMI_Suspend(&hdcmi);
    HAL_DCMI_Stop(&hdcmi);
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
