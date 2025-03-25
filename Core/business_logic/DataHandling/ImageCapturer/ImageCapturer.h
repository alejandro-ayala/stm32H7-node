#pragma once
#include "hardware_abstraction/Devices/Ov2460/Ov2640Ctrl.h"
#include "business_logic/Osal/QueueHandler.h"
#include "../Middlewares/Third_Party/LibJPEG/include/jpeglib.h"
#include "EdgeDetector.h"
#include "JpegCompressor.h"
#include "IEncoder.h"
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
	std::shared_ptr<EdgeDetector> m_edgeDetector;
	std::shared_ptr<JpegCompressor> m_imageCompressor;
	std::shared_ptr<IEncoder> m_imageEncoder;

	std::unique_ptr<uint8_t[]> m_pic;
	ImageState m_imageState;
	size_t m_picSize;
	ImageConfiguration m_imageConfig;

public:
	explicit ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl);
	virtual ~ImageCapturer();

    void initialize();
    void stop();
	void captureImage();
	void extractImage();
	size_t getRawImageBufferSize() const;
	const uint8_t* getRawImageBuffer() const;
	unsigned long processEdges(const uint8_t* image, uint8_t*& edges, size_t size);
	uint32_t getBufferSize() const;
	void encodeEdgesImage(uint8_t* initialImage, const uint32_t initialImageSize, std::vector<RLEFrame>& encoded ) const;

};
}
}
