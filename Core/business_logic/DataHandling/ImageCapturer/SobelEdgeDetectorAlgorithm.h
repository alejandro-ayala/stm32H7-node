#pragma once
#include <iostream>
#include <iostream>
#include <cmath>
#include <array>

class SobelEdgeDetectorAlgorithm
{
private:

    // Función para aplicar el kernel de convolución a una región de la imagen
	void sobelGradient(const std::array<std::array<uint8_t, imgWidth>, imgHeight> &image, std::array<std::array<uint8_t, imgWidth>, imgHeight> &edges)
	{
		const std::array<std::array<int, 3>, 3> sobel_x = {{
		    {{-1, 0, 1}},
		    {{-2, 0, 2}},
		    {{-1, 0, 1}}
		}};

		const std::array<std::array<int, 3>, 3> sobel_y = {{
		    {{-1, -2, -1}},
		    {{0, 0, 0}},
		    {{1, 2, 1}}
		}};
	    for (size_t i = 1; i < imgHeight - 1; ++i)
	    {
	        for (size_t j = 1; j < imgWidth - 1; ++j)
	        {
	            // Crear una región 3x3 alrededor del píxel actual
	            std::array<std::array<int, 3>, 3> region = {{
	                {image[i - 1][j - 1], image[i - 1][j], image[i - 1][j + 1]},
	                {image[i][j - 1], image[i][j], image[i][j + 1]},
	                {image[i + 1][j - 1], image[i + 1][j], image[i + 1][j + 1]}
	            }};

	            // Calcular gradientes en dirección x e y
	            int gx = applyKernel(sobel_x, region);
	            int gy = applyKernel(sobel_y, region);

	            // Calcular la magnitud del gradiente
	            edges[i][j] = sqrt(pow(gx, 2) + pow(gy, 2));
	        }
	    }
	}

	int applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region)
	{
	    int sum = 0;
	    const auto a = kernel.size();
	    const auto b = kernel[0].size();

	    for (size_t i = 0; i < a; ++i)
	    {
	        for (size_t j = 0; j < b; ++j)
	        {
	        	auto size = kernel.size();
	        	auto size2 = kernel.at(i).size();
	        	const auto reg = region.at(i).at(j);
	        	const auto ker = kernel.at(i).at(j);

	            sum += ker * reg;
	        }
	    }
	    return sum;
	}


public:
    void process(const std::array<std::array<uint8_t, imgWidth>, imgHeight> &image, std::array<std::array<uint8_t, imgWidth>, imgHeight> &edges)
    {
        const auto height = image.size();
        const auto width = image.at(0).size();
        const auto sizeImg = height * width;
        std::cout << "SobelEdgeDetectorAlgorithm executed for image of size:" << std::to_string(sizeImg) << std::to_string(height) << "x" << std::to_string(width) << std::endl;

        sobelGradient(image, edges);
    }
};
