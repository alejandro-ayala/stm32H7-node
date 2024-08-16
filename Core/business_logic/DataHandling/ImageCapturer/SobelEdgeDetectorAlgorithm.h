#pragma once
#include "IEdgeDetectorAlgorithm.h"
#include "ImageConfiguration.h"
#include <iostream>
#include <cmath>
#include <array>

namespace business_logic
{
namespace DataHandling
{
static const std::array<std::array<int, 3>, 3> sobel_x = {{
    {{-1, 0, 1}},
    {{-2, 0, 2}},
    {{-1, 0, 1}}
}};

static const std::array<std::array<int, 3>, 3> sobel_y = {{
    {{-1, -2, -1}},
    {{0, 0, 0}},
    {{1, 2, 1}}
}};

class SobelEdgeDetectorAlgorithm : public IEdgeDetectorAlgorithm
{
private:
	void sobelGradient(const  uint8_t* image, uint8_t*  edges);
	int applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region);

	uint32_t m_imgHeight;
	uint32_t m_imgWidth;
public:
	explicit SobelEdgeDetectorAlgorithm(ImageConfiguration imageConfig);
    void process(const uint8_t* rawImage, uint8_t* edges, size_t size) override;
};

}
}
