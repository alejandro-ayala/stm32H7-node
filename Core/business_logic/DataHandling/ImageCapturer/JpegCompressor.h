#pragma once
#include "../Middlewares/Third_Party/LibJPEG/include/jpeglib.h"
#include "ImageState.h"

namespace business_logic
{
namespace DataHandling
{
class JpegCompressor
{
private:

	void setupDecodeJPEG(uint8_t *jpegImg, uint16_t buffer_length,uint8_t greyscale, ImageState& imageState, struct jpeg_decompress_struct& cinfo, struct jpeg_error_mgr& jerr);
	void setupEncodeJPEG(uint8_t **image_buffer, unsigned long *image_size, uint8_t image_quality, ImageState imageState, struct jpeg_compress_struct& cinfo, struct jpeg_error_mgr& jerr);

public:
	JpegCompressor() = default;
	virtual ~JpegCompressor() = default;

	unsigned long decompress(uint8_t *jpegImg, uint16_t jpegImgSize, uint8_t *rawImg, size_t& rawImgSize, uint8_t greyscale, ImageState& imageState);
	void compress(uint8_t *rawImg, uint8_t **compressedImg, unsigned long *compressedImgSize, uint8_t compressedImgQuality, ImageState imageState);

};
}
}
