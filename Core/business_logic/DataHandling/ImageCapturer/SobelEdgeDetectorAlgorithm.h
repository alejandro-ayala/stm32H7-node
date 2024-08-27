#pragma once

//#define SIMD

#ifdef SIMD
#include <vector>
extern "C" {
#include <arm_math.h>  //CMSIS-DSP
}
#endif

#include "IEdgeDetectorAlgorithm.h"
#include "ImageConfiguration.h"
#include <iostream>
#include <cmath>
#include <array>

namespace business_logic
{
namespace DataHandling
{

class SobelEdgeDetectorAlgorithm : public IEdgeDetectorAlgorithm
{
private:
	void sobelGradient(const  uint8_t* image, uint8_t*  edges);
#ifndef SIMD
	int applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region);
#else
	float32_t applyKernel(float32_t *kernel, float32_t *region);
#endif
	uint8_t pixelBinarization(uint8_t originalValue) const;
	uint32_t m_imgHeight;
	uint32_t m_imgWidth;
public:
	explicit SobelEdgeDetectorAlgorithm(ImageConfiguration imageConfig);
    void process(const uint8_t* rawImage, uint8_t* edges, size_t size) override;
};

}
}
