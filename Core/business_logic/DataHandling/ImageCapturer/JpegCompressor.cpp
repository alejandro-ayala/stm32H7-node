#include "JpegCompressor.h"
#include <stdexcept>
#include <vector>
#include <memory>

namespace business_logic
{
namespace DataHandling
{

JpegCompressor::JpegCompressor()
{
	m_cinfo_comp.err = jpeg_std_error(&m_jerr);
    jpeg_create_compress(&m_cinfo_comp);

    m_cinfo_decomp.err = jpeg_std_error(&m_jerr);
    jpeg_create_decompress(&m_cinfo_decomp);
}

JpegCompressor::~JpegCompressor()
{
    jpeg_destroy_decompress(&m_cinfo_decomp);
    jpeg_destroy_compress(&m_cinfo_comp);

}

void JpegCompressor::setupDecodeJPEG(uint8_t *jpegImg, uint16_t buffer_length, uint8_t greyscale, ImageState& imageState)
{

    jpeg_mem_src(&m_cinfo_decomp, jpegImg, buffer_length);

    if (jpeg_read_header(&m_cinfo_decomp, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&m_cinfo_decomp);
        throw std::runtime_error("Error al leer el encabezado JPEG");
    }

    jpeg_start_decompress(&m_cinfo_decomp);

    imageState.image_width = m_cinfo_decomp.output_width;
    imageState.image_heigh = m_cinfo_decomp.output_height;

    if (greyscale == 1 && m_cinfo_decomp.num_components == 3)
    {
        imageState.image_byte_per_pixel = 1;
        imageState.grayscale = 1;
    }
    else
    {
        imageState.image_byte_per_pixel = m_cinfo_decomp.num_components;
        imageState.grayscale = (m_cinfo_decomp.num_components == 1) ? 1 : 0;
    }
}
unsigned long JpegCompressor::decompress(uint8_t *jpegImg, uint16_t jpegImgSize, uint8_t *rawImg, size_t& rawImgSize, uint8_t greyscale, ImageState& imageState)
{
    setupDecodeJPEG(jpegImg, jpegImgSize, greyscale, imageState);

    std::unique_ptr<uint8_t[]> row_buffer = std::make_unique<uint8_t[]>(m_cinfo_decomp.output_width * m_cinfo_decomp.num_components);
    JSAMPROW row_pointer[1];
    row_pointer[0] = row_buffer.get();

#ifdef FLOAT_ENABLE
    float value_red, value_green, value_blue;
#else
    uint8_t value_red, value_green, value_blue;
#endif

    uint8_t pixel_number = 0;
    unsigned long pixel_location = 0;
    while (m_cinfo_decomp.output_scanline < m_cinfo_decomp.image_height)
    {
        jpeg_read_scanlines(&m_cinfo_decomp, row_pointer, 1);
        for (short i = 0; i < m_cinfo_decomp.image_width * m_cinfo_decomp.num_components; i++)
        {
            if (greyscale == 1)
            {
                pixel_number++;
                if (pixel_number == 1) value_red = row_pointer[0][i];
                if (pixel_number == 2) value_green = row_pointer[0][i];
                if (pixel_number == 3)
                {
                    value_blue = row_pointer[0][i];
#ifdef FLOAT_ENABLE
                    rawImg[pixel_location++] = (value_red * 0.299
                            + value_green * 0.587 + value_blue * 0.114);
#else
                    rawImg[pixel_location++] = static_cast<uint8_t>(value_red / 3 + value_green / 3 + value_blue / 3);
#endif
                    pixel_number = 0;
                }
            }
            else
            {
                rawImg[pixel_location++] = static_cast<uint8_t>(row_pointer[0][i]);
            }
        }
    }

    rawImgSize = pixel_location;
    jpeg_finish_decompress(&m_cinfo_decomp);

    return pixel_location;
}

void JpegCompressor::setupEncodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality, ImageState imageState)
{
    jpeg_mem_dest(&m_cinfo_comp, image_buffer, image_size);

    m_cinfo_comp.image_width = imageState.image_width;
    m_cinfo_comp.image_height = imageState.image_heigh;
    m_cinfo_comp.input_components = imageState.image_byte_per_pixel;
    m_cinfo_comp.in_color_space = imageState.grayscale ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&m_cinfo_comp);
    jpeg_set_quality(&m_cinfo_comp, image_quality, TRUE);
    jpeg_start_compress(&m_cinfo_comp, TRUE);
}

void JpegCompressor::compress(uint8_t *rawImg, uint8_t **compressedImg, unsigned long *compressedImgSize, uint8_t compressedImgQuality, ImageState imageState)
{
	JSAMPROW row_pointer[1];

	setupEncodeJPEG(compressedImg, compressedImgSize, compressedImgQuality, imageState);

	while (m_cinfo_comp.next_scanline < m_cinfo_comp.image_height)
	{
		row_pointer[0] = &rawImg[m_cinfo_comp.next_scanline * m_cinfo_comp.image_width];
		jpeg_write_scanlines(&m_cinfo_comp, row_pointer, 1);
	}

	jpeg_finish_compress( &m_cinfo_comp );
	delete[] rawImg;
}
}
}
