#pragma once
#include "hardware_abstraction/Devices/Ov2460/Ov2640Ctrl.h"
#include "business_logic/Osal/QueueHandler.h"
#include "../Middlewares/Third_Party/LibJPEG/include/jpeglib.h"
#include "EdgeDetector.h"
#include "ImageState.h"
#include <memory>

namespace business_logic
{
namespace DataHandling
{
class ImageCapturer
{
private:
	std::shared_ptr<hardware_abstraction::Devices::ICameraDevice> m_cameraControl;
	std::shared_ptr<business_logic::Osal::QueueHandler> m_capturesQueue;
	std::shared_ptr<EdgeDetector> m_edgeDetector;
	uint8_t* m_pic;
	ImageState m_imageState;
	size_t m_picSize;
	ImageConfiguration m_imageConfig;

	void setupDecodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale, struct jpeg_decompress_struct& cinfo, struct jpeg_error_mgr& jerr);
	void setupEncodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality, struct jpeg_compress_struct& cinfo, struct jpeg_error_mgr& jerr);

public:
	explicit ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl, const std::shared_ptr<business_logic::Osal::QueueHandler> capturesQueue = nullptr);
	virtual ~ImageCapturer();

    void initialize();
    void stop();
	void captureImage();
	void extractImage();
	void decodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale);
	void encodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality);
	size_t getRawImageBufferSize() const;
	const uint8_t* getRawImageBuffer() const;
	void processEdges(const uint8_t* image, uint8_t* edges, size_t size);
	uint32_t getBufferSize() const;
};
}
}
