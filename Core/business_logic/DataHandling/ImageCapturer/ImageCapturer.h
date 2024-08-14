#pragma once
#include "hardware_abstraction/Devices/Ov2460/Ov2640Ctrl.h"
#include "business_logic/Osal/QueueHandler.h"
#include "EdgeDetector.h"
#include <memory>
#include <array>

namespace business_logic
{
namespace DataHandling
{
typedef struct
{
	 short image_width;
	 short image_heigh;
	 uint8_t image_byte_per_pixel;
	 uint8_t grayscale;
}ImageStateHandle;

class ImageCapturer
{
private:
	std::shared_ptr<hardware_abstraction::Devices::ICameraDevice> m_cameraControl;
	std::shared_ptr<business_logic::Osal::QueueHandler> m_capturesQueue;
	std::shared_ptr<EdgeDetector> m_edgeDetector;
	void arrangeBuffer(const unsigned char *image, std::array<std::array<uint8_t, imgWidth>, imgHeight>& rawImage);
	uint8_t* m_pic;
	ImageStateHandle imageState;
	size_t m_picSize;
public:
	explicit ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl, const std::shared_ptr<business_logic::Osal::QueueHandler> capturesQueue = nullptr);
	virtual ~ImageCapturer();

    void initialize();
    void stop();
	void captureImage();
	void extractImage();
	void decodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale);
	//void decodeJPEG(const uint8_t *image_buffer, uint16_t buffer_length, uint8_t greyscale);
	void encodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality);
	size_t getRawImageBufferSize() const;
	const uint8_t* getRawImageBuffer() const;
	void processEdges(const uint8_t* image, size_t size);
};
}
}
