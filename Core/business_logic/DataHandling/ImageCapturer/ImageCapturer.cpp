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

ImageCapturer::ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl, const std::shared_ptr<business_logic::Osal::QueueHandler> capturesQueue) : m_cameraControl(cameraControl), m_capturesQueue(capturesQueue)
{
	m_imageConfig = cameraControl->getImageResolution();
	m_pic   = new uint8_t[m_imageConfig.imageWidth * m_imageConfig.imageHeight];
	if(m_pic == nullptr)
	{
		std::cout << "Memory allocation for m_pic failed" << std::endl;
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::MemoryAllocationFailed, "Memory allocation for m_pic failed");
	}

	m_edgeDetector = std::make_shared<EdgeDetector>(std::make_shared<SobelEdgeDetectorAlgorithm>(m_imageConfig));
	//BUSINESS_LOGIC_ASSERT( m_capturesQueue->getAvailableSpace() != 0, services::BusinessLogicErrorId::QueueIsFull, "Queue to store the camera images is full");
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
        const uint8_t* bufferAddr = m_cameraControl->getImageBuffer();
        const size_t bufferSize = m_cameraControl->getImageBufferSize();
		decodeJPEG(const_cast<uint8_t*>(bufferAddr), bufferSize, 1);
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

void ImageCapturer::setupDecodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale, struct jpeg_decompress_struct& cinfo, struct jpeg_error_mgr& jerr)
{
	m_picSize = 0;
	cinfo.err = jpeg_std_error(&jerr);


	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, image_buffer, buffer_length);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	m_imageState.image_width = cinfo.output_width;
	m_imageState.image_heigh = cinfo.output_height;

	if (greyscale == 1)
	{
		m_imageState.image_byte_per_pixel = 1;
		m_imageState.grayscale=1;
	}
	else
	{
		m_imageState.image_byte_per_pixel = cinfo.num_components;
		m_imageState.grayscale=0;
	}

}

void ImageCapturer::decodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale)
{
	struct jpeg_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	setupDecodeJPEG(image_buffer, buffer_length, greyscale, cinfo, jerr);

	JSAMPROW row_pointer[1];
	row_pointer[0] = (unsigned char*) malloc(cinfo.output_width * cinfo.num_components);

#ifdef FLOAT_ENABLE
	float value_red, value_green, value_blue;
#else
		uint8_t value_red, value_green, value_blue;
#endif

	uint8_t pixel_number = 0;
	unsigned long pixel_location = 0;
	while (cinfo.output_scanline < cinfo.image_height)
	{
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for (short i = 0; i < cinfo.image_width * cinfo.num_components; i++)
		{
			if(greyscale==1)
			{
				pixel_number++;
				if (pixel_number == 1) {
					value_red = row_pointer[0][i];
				}
				if (pixel_number == 2) {
					value_green = row_pointer[0][i];
				}
				if (pixel_number == 3) {
					value_blue = row_pointer[0][i];
	#ifdef FLOAT_ENABLE
					m_pic[pixel_location++] = (value_red * 0.299
							+ value_green * 0.587 + value_blue * 0.114);
	#else
					m_pic[pixel_location++] = static_cast<uint8_t>(value_red/3+value_green/3+value_blue/3);
	#endif
					pixel_number = 0;
				}
			}
			else
			{
				m_pic[pixel_location++] = static_cast<uint8_t>(row_pointer[0][i]);
			}
		}
	}
	m_picSize = pixel_location;
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
}

void ImageCapturer::setupEncodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality, struct jpeg_compress_struct& cinfo, struct jpeg_error_mgr& jerr)
{
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	cinfo.image_width = m_imageState.image_width;
	cinfo.image_height = m_imageState.image_heigh;
	cinfo.input_components = m_imageState.image_byte_per_pixel;
	if(m_imageState.grayscale==1)
	{
		cinfo.in_color_space = JCS_GRAYSCALE;
	}
	else
	{
		cinfo.in_color_space = JCS_RGB;
	}
	jpeg_set_defaults( &cinfo );
	jpeg_set_quality(&cinfo, image_quality, TRUE );
	jpeg_mem_dest(&cinfo, image_buffer, image_size);
	jpeg_start_compress( &cinfo, TRUE );
}

void ImageCapturer::encodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	setupEncodeJPEG(image_buffer, image_size, image_quality, cinfo, jerr);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &m_pic[cinfo.next_scanline * cinfo.image_width];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	free(m_pic);
}

const uint8_t* ImageCapturer::getRawImageBuffer() const
{
    return m_pic;
}

size_t ImageCapturer::getRawImageBufferSize() const
{
    return m_picSize;
}

void ImageCapturer::processEdges(const  uint8_t* image, uint8_t* edges, size_t size)
{
	m_edgeDetector->processImage(image, edges, size);
	memcpy(m_pic, edges, size);
	std::memset(edges, 0, size);

	unsigned long compressedSize = 0;
	encodeJPEG(&edges, &compressedSize, m_imageConfig.imageQuality);
}

uint32_t ImageCapturer::getBufferSize() const
{
	return m_imageConfig;
}
}
}
