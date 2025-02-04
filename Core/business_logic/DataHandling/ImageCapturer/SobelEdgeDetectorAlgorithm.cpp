#include "SobelEdgeDetectorAlgorithm.h"
#include "services/Logger/LoggerMacros.h"

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
#ifndef SIMD
            // Crear una región 3x3 alrededor del píxel actual
            const std::array<std::array<int, 3>, 3> region = {{
                {image[(i - 1) * m_imgWidth + (j - 1)], image[(i - 1) * m_imgWidth + j], image[(i - 1) * m_imgWidth + (j + 1)]},
                {image[i * m_imgWidth + (j - 1)], image[i * m_imgWidth + j], image[i * m_imgWidth + (j + 1)]},
                {image[(i + 1) * m_imgWidth + (j - 1)], image[(i + 1) * m_imgWidth + j], image[(i + 1) * m_imgWidth + (j + 1)]}
            }};

            // Calcular gradientes en dirección x e y

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

            int gx = applyKernel(sobel_x, region);
            int gy = applyKernel(sobel_y, region);

            // Calcular la magnitud del gradiente
			const auto pixelValue = sqrt(pow(gx, 2) + pow(gy, 2));
			//Binarizacion de imagen
#else
	           // Crear una región 3x3 alrededor del píxel actual
			float32_t region[9] = {
				(float32_t)image[(i - 1) * m_imgWidth + (j - 1)], (float32_t)image[(i - 1) * m_imgWidth + j], (float32_t)image[(i - 1) * m_imgWidth + (j + 1)],
				(float32_t)image[i * m_imgWidth + (j - 1)], (float32_t)image[i * m_imgWidth + j], (float32_t)image[i * m_imgWidth + (j + 1)],
				(float32_t)image[(i + 1) * m_imgWidth + (j - 1)], (float32_t)image[(i + 1) * m_imgWidth + j], (float32_t)image[(i + 1) * m_imgWidth + (j + 1)]
			};

			// Calcular gradientes en dirección x e y usando los kernels Sobel
			float32_t sobel_x[9] = {
			    -1, 0, 1,
			    -2, 0, 2,
			    -1, 0, 1
			};

			float32_t sobel_y[9] = {
			    -1, -2, -1,
			     0,  0,  0,
			     1,  2,  1
			};
			float32_t gx = applyKernel(sobel_x, region);
			float32_t gy = applyKernel(sobel_y, region);

			// Calcular la magnitud del gradiente usando funciones DSP
			float32_t gx_squared, gy_squared, sum_squared, pixelValue;
			arm_mult_f32(&gx, &gx, &gx_squared, 1); // gx_squared = gx^2
			arm_mult_f32(&gy, &gy, &gy_squared, 1); // gy_squared = gy^2
			arm_add_f32(&gx_squared, &gy_squared, &sum_squared, 1); // sum_squared = gx^2 + gy^2
			arm_sqrt_f32(sum_squared, &pixelValue); // pixelValue = sqrt(gx^2 + gy^2)

#endif
	    	edges[i * m_imgWidth + j] = pixelBinarization(pixelValue);
        }
    }
}

#ifndef SIMD
int SobelEdgeDetectorAlgorithm::applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region)
{

// Realiza una convolucion simple entre los arrays region y kernel
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
#else
float32_t SobelEdgeDetectorAlgorithm::applyKernel(float32_t *dspKernel, float32_t *dspRegion)
{
// Con el DSP utilizamos las funciones de la libreria arm_math para realizar operaciones matriciales

    //TODO check the matrix can be multiplied and refactor magic number
    const auto A_SIZE =  3;
    const auto B_SIZE = 3;
//    float32_t dspRegion[A_SIZE * B_SIZE] = { /* datos de la matriz region */ };
//    float32_t dspKernel[A_SIZE * B_SIZE] = { /* datos de la matriz kernel */ };
      float32_t dspResult[A_SIZE * B_SIZE];
//
//    for (size_t i = 0; i < A_SIZE; ++i) {
//        for (size_t j = 0; j < B_SIZE; ++j) {
//        	dspRegion[i * B_SIZE + j] = (float32_t)region[i][j];
//        	dspKernel[i * B_SIZE + j] = (float32_t)kernel[i][j];
//        }
//    }
    // Inicializacion de las matrices
    arm_matrix_instance_f32 matrixRegion;
    arm_matrix_instance_f32 matrixKernel;
    arm_matrix_instance_f32 matrixResult;

    arm_mat_init_f32(&matrixRegion, A_SIZE, B_SIZE, dspRegion);
    arm_mat_init_f32(&matrixKernel, A_SIZE, B_SIZE, dspKernel);
    arm_mat_init_f32(&matrixResult, A_SIZE, B_SIZE, dspResult);

    arm_status status;

    // Multiplica las matrices
    status = arm_mat_mult_f32(&matrixRegion, &matrixKernel, &matrixResult);

    if (status != ARM_MATH_SUCCESS)
    {
        // Manejar error
        return 0;
    }

    // Sumar todos los elementos de la matriz resultado
    float32_t sum = 0;
    for (int i = 0; i < A_SIZE * B_SIZE; ++i)
    {
        sum += dspResult[i];
    }
    /* USER CODE END PD */

    return 0;
#endif
}

void SobelEdgeDetectorAlgorithm::process(const uint8_t* rawImage, uint8_t* edges, size_t size)
{
	LOG_DEBUG("SobelEdgeDetectorAlgorithm executed for image of size: ", size);
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
