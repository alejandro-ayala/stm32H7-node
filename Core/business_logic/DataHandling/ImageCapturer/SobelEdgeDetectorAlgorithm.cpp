#include "SobelEdgeDetectorAlgorithm.h"

namespace business_logic
{
namespace DataHandling
{

SobelEdgeDetectorAlgorithm::SobelEdgeDetectorAlgorithm(ImageConfiguration imgCfg) : m_imgHeight(imgCfg.imageHeight), m_imgWidth(imgCfg.imageWidth)
{

}

void SobelEdgeDetectorAlgorithm::sobelGradient(const  uint8_t* image, uint8_t*  edges)
{
    for (size_t i = 1; i < m_imgHeight - 1; ++i)
    {
        for (size_t j = 1; j < m_imgWidth - 1; ++j)
        {
            // Crear una región 3x3 alrededor del píxel actual
            const std::array<std::array<int, 3>, 3> region = {{
                {image[(i - 1) * m_imgWidth + (j - 1)], image[(i - 1) * m_imgWidth + j], image[(i - 1) * m_imgWidth + (j + 1)]},
                {image[i * m_imgWidth + (j - 1)], image[i * m_imgWidth + j], image[i * m_imgWidth + (j + 1)]},
                {image[(i + 1) * m_imgWidth + (j - 1)], image[(i + 1) * m_imgWidth + j], image[(i + 1) * m_imgWidth + (j + 1)]}
            }};
            // Calcular gradientes en dirección x e y
            int gx = applyKernel(sobel_x, region);
            int gy = applyKernel(sobel_y, region);

            // Calcular la magnitud del gradiente
			const auto pixelValue = sqrt(pow(gx, 2) + pow(gy, 2));
			//Binarizacion de imagen
			//edges[i * m_imgWidth + j] = pixelValue;//pixelBinarization(pixelValue);
			edges[i * m_imgWidth + j] = pixelBinarization(pixelValue);
        }
    }
}

int SobelEdgeDetectorAlgorithm::applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region)
{
    int sum = 0;
    const auto a = kernel.size();
    const auto b = kernel[0].size();

    for (size_t i = 0; i < a; ++i)
    {
        for (size_t j = 0; j < b; ++j)
        {
            sum += region.at(i).at(j) * kernel.at(i).at(j);
        }
    }
    return sum;
}

void SobelEdgeDetectorAlgorithm::process(const uint8_t* rawImage, uint8_t* edges, size_t size)
{
    std::cout << "SobelEdgeDetectorAlgorithm executed for image of size:" << std::to_string(size) << std::endl;
    sobelGradient(rawImage, edges);
}

uint8_t SobelEdgeDetectorAlgorithm::pixelBinarization(uint8_t originalValue) const
{
	static uint8_t threshold = 55;
	static uint8_t whitePixel = 255;
	static uint8_t blackPixel = 0;

	if(originalValue >= threshold)
	{
		return whitePixel;
	}
	else
	{
		return blackPixel;
	}
}
}
}
