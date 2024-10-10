#include "RLEEncoder.h"

#include "business_logic/DataSerializer/ImageSnapshot.h"
#include "../../DataSerializer/DataSerializer.h"
#include "ImageCapturer.h"
#include "services/Exception/SystemExceptions.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"
#include <cstring>

#define FLOAT_ENABLE

namespace business_logic
{
namespace DataHandling
{

ImageCapturer::ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl) : m_cameraControl(cameraControl)
{
	m_imageConfig = m_cameraControl->getImageResolution();
	m_pic   = new uint8_t[m_imageConfig.imageWidth * m_imageConfig.imageHeight];
	if(m_pic == nullptr)
	{
		std::cout << "Memory allocation for m_pic failed" << std::endl;
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::MemoryAllocationFailed, "Memory allocation for m_pic failed");
	}

	m_edgeDetector = std::make_shared<EdgeDetector>(std::make_shared<SobelEdgeDetectorAlgorithm>(m_imageConfig));
	//BUSINESS_LOGIC_ASSERT( m_capturesQueue->getAvailableSpace() != 0, services::BusinessLogicErrorId::QueueIsFull, "Queue to store the camera images is full");
	m_imageEncoder   = std::make_shared<RLEEncoder>();

	m_imageCompressor = std::make_shared<JpegCompressor>();
}

ImageCapturer::~ImageCapturer()
{
    delete[] m_pic;
}

void ImageCapturer::initialize()
{
	m_cameraControl->initialization();
	m_cameraControl->configuration(hardware_abstraction::Devices::CameraResolution::RES_320X240);
}

void ImageCapturer::captureImage()
{
	m_cameraControl->captureSnapshot();
	//TODO add a time check for ensure that camera is not blocked during capture process.

}

void ImageCapturer::extractImage()
{
	if(/*!m_cameraControl->isCapturingFrame()*/true)
	{
        const uint8_t* jpegImage = m_cameraControl->getImageBuffer();
        const size_t jpegImgSize = m_cameraControl->getImageBufferSize();
        const uint8_t greyscale = 1;
		//decodeJPEG(const_cast<uint8_t*>(bufferAddr), bufferSize, 1);
		m_imageCompressor->decompress(const_cast<uint8_t*>(jpegImage), jpegImgSize, m_pic, m_picSize, greyscale, m_imageState);
	}
	else
	{
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::CameraTimeout, "Camera timeout while capturing image");
	}
}


void ImageCapturer::stop()
{
	m_cameraControl->stopCapture();
}

const uint8_t* ImageCapturer::getRawImageBuffer() const
{
    return m_pic;
}

size_t ImageCapturer::getRawImageBufferSize() const
{
    return m_picSize;
}

unsigned long ImageCapturer::processEdges(const  uint8_t* image, uint8_t*& edges, size_t size)
{
	m_edgeDetector->processImage(image, edges, size);

#ifdef RLE_ENCODER
	this->encodeEdgesImage(const_cast<uint8_t*>(edges), 76800, encodedImg);

	size_t encodedImgSize = encodedImg.size() * sizeof(RLEFrame);

	std::memset(edges, 0, size);
    //std::memcpy(edges, reinterpret_cast<const uint8_t*>(encodedImg.data()), encodedImgSize);

	uint32_t j = 0;
	for(const auto& element : encodedImg)
	{
		for(uint32_t i = 0; i< element.pixelRepetition; i++)
		{
			edges[j] = element.pixelValue;
			j++;
		}
	}
#else
	memcpy(m_pic, edges, size);
	std::memset(edges, 0, size);

	unsigned long compressedSize = 0;
	m_imageCompressor->compress(m_pic, &edges, &compressedSize, m_imageConfig.imageQuality, m_imageState);
	std::cout << " Compressed edges image with" << std::endl;

//	business_logic::DataSerializer::ImageSnapshot edgesSnapshot2;
//	business_logic::DataSerializer::ImageSnapshot *pxPointerToxMessage = nullptr;
//	m_capturesQueue->receive(&pxPointerToxMessage);
//	edgesSnapshot2 = *pxPointerToxMessage;

	return compressedSize;
#endif
}

uint32_t ImageCapturer::getBufferSize() const
{
	return m_imageConfig;
}

void ImageCapturer::encodeEdgesImage(uint8_t* initialImage, const uint32_t initialImageSize , std::vector<RLEFrame>& encoded) const
{
	m_imageEncoder->encode(initialImage, initialImageSize, encoded);
}
}
}
