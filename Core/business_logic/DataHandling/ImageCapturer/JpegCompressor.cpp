#include "JpegCompressor.h"


namespace business_logic
{
namespace DataHandling
{

void JpegCompressor::setupDecodeJPEG(uint8_t *jpegImg, uint16_t buffer_length,uint8_t greyscale, ImageState& imageState, struct jpeg_decompress_struct& cinfo, struct jpeg_error_mgr& jerr)
{
	cinfo.err = jpeg_std_error(&jerr);


	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, jpegImg, buffer_length);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	imageState.image_width = cinfo.output_width;
	imageState.image_heigh = cinfo.output_height;

	if (greyscale == 1)
	{
		imageState.image_byte_per_pixel = 1;
		imageState.grayscale=1;
	}
	else
	{
		imageState.image_byte_per_pixel = cinfo.num_components;
		imageState.grayscale=0;
	}

}

unsigned long JpegCompressor::decompress(uint8_t *jpegImg, uint16_t jpegImgSize, uint8_t *rawImg, size_t& rawImgSize, uint8_t greyscale, ImageState& imageState)
{
	struct jpeg_error_mgr jerr;
	struct jpeg_decompress_struct cinfo;
	setupDecodeJPEG(jpegImg, jpegImgSize, greyscale, imageState, cinfo, jerr);

	JSAMPROW row_pointer[1];
	//row_pointer[0] = (unsigned char*) malloc(cinfo.output_width * cinfo.num_components);
	row_pointer[0]   = new unsigned char[cinfo.output_width * cinfo.num_components];

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
					rawImg[pixel_location++] = (value_red * 0.299
							+ value_green * 0.587 + value_blue * 0.114);
	#else
					rawImg[pixel_location++] = static_cast<uint8_t>(value_red/3+value_green/3+value_blue/3);
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
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	delete[] row_pointer[0];
	return pixel_location;
}

void JpegCompressor::setupEncodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality, ImageState imageState, struct jpeg_compress_struct& cinfo, struct jpeg_error_mgr& jerr)
{
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
}

void JpegCompressor::compress(uint8_t *rawImg, uint8_t **compressedImg, unsigned long *compressedImgSize, uint8_t compressedImgQuality, ImageState imageState)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];

	setupEncodeJPEG(compressedImg, compressedImgSize, compressedImgQuality, imageState, cinfo, jerr);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &rawImg[cinfo.next_scanline * cinfo.image_width];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	delete[] rawImg;
}
}
}
