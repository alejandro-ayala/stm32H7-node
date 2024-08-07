#include "ImageCapturer.h"
#include "services/Exception/SystemExceptions.h"
#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"
#include "jpeg.h"


namespace business_logic
{
namespace DataHandling
{

ImageCapturer::ImageCapturer(const std::shared_ptr<hardware_abstraction::Devices::ICameraDevice>& cameraControl, const std::shared_ptr<business_logic::Osal::QueueHandler> capturesQueue) : m_cameraControl(cameraControl), m_capturesQueue(capturesQueue)
{
	//m_edges = new double*[imgHeight];
	m_pic   = new uint8_t[imgWidth * imgHeight];
	if(m_pic == nullptr)
	{
		std::cout << "Memory allocation for m_pic failed" << std::endl;
	}

	m_edgeDetector = std::make_shared<EdgeDetector>();
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
		//unsigned char imageBuffer[320*240];
        const uint8_t* bufferAddr = m_cameraControl->getImageBuffer();
        size_t bufferSize = m_cameraControl->getImageBufferSize();
		//decodeJPEG(bufferAddr, bufferSize, 1);
		decodeJPEG(const_cast<uint8_t*>(bufferAddr), bufferSize, 1);
	}
	else
	{
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::CameraTimeout, "Camera timeout while capturing image");
	}
}

void ImageCapturer::arrangeBuffer(const unsigned char *image, std::array<std::array<uint8_t, imgWidth>, imgHeight>& rawImage)
{

    if (image != nullptr)
    {
        const auto dataSize = imgWidth * imgHeight;
        std::cout << "Image load correctly. Size: " << std::to_string(dataSize) << std::endl;

        std::cout << "m_rawImageSize: " << std::to_string(rawImage.size()) << std::endl;

        for (int i = 0; i < imgHeight; ++i)
        {
            for (int j = 0; j < imgWidth; ++j)
            {
                // Calcular el índice en el arreglo unidimensional
                int index = i * imgWidth + j;
                // Asignar el valor del píxel a la matriz bidimensional
                rawImage[i][j] = static_cast<uint8_t>(image[index]);
            }
        }
        std::cout << "Mapper image to array: " << std::to_string(rawImage.size()) << " rows of " << std::to_string(rawImage.at(0).size()) << " elements" << std::endl;
    }
    else
    {
        std::cerr << "Error al cargar la imagen." << std::endl;
    }
}

void ImageCapturer::stop()
{
	m_cameraControl->stopCapture();
}

//void ImageCapturer::decodeJPEG(const uint8_t *image_buffer, uint16_t buffer_length, uint8_t greyscale)
void ImageCapturer::decodeJPEG(uint8_t *image_buffer, uint16_t buffer_length,uint8_t greyscale)
{

	uint8_t pixel_number = 0;
	unsigned long pixel_location = 0;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	cinfo.err = jpeg_std_error(&jerr);

#ifdef FLOAT_ENABLE
	float value_red, value_green, value_blue;
#else
		uint8_t value_red, value_green, value_blue;
#endif

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, image_buffer, buffer_length);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	imageState.image_width = cinfo.output_width;
	imageState.image_heigh = cinfo.output_height;

	if (greyscale == 1)
	{

		//image_RGB = (uint8_t*) malloc(cinfo.output_width * cinfo.output_height*1);
		imageState.image_byte_per_pixel = 1;
		imageState.grayscale=1;
	}
	else
	{
		// Allocation of very large amount of memory, for 320x240 in RGB format (3 bits per pixel) it is around 230.4 kilobytes ram memory!
		//image_RGB = (uint8_t*) malloc(cinfo.output_width * cinfo.output_height * 3);
		imageState.image_byte_per_pixel = cinfo.num_components;
		imageState.grayscale=0;
	}

	row_pointer[0] = (unsigned char*) malloc(cinfo.output_width * cinfo.num_components);

	while (cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for (short i = 0; i < cinfo.image_width * cinfo.num_components; i++) {

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
					image_RGB[pixel_location++] = (value_red * 0.299
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
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);

}

void ImageCapturer::encodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality)
{
	struct jpeg_compress_struct cinfo;
		struct jpeg_error_mgr jerr;
		JSAMPROW row_pointer[1];
		cinfo.err = jpeg_std_error( &jerr );
		jpeg_create_compress(&cinfo);
		cinfo.image_width = imageState.image_width;
		cinfo.image_height = imageState.image_heigh;
		cinfo.input_components = imageState.image_byte_per_pixel;
		if(imageState.grayscale==1)
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
		while (cinfo.next_scanline < cinfo.image_height) {
				row_pointer[0] = &m_pic[cinfo.next_scanline * cinfo.image_width];
				jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}

		jpeg_finish_compress( &cinfo );
		jpeg_destroy_compress( &cinfo );
		free(m_pic);
}

}
}
