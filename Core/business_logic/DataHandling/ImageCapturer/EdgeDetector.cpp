#include "EdgeDetector.h"

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
EdgeDetector::EdgeDetector(const std::shared_ptr<SobelEdgeDetectorAlgorithm> &detectionEdgeAlgo) : m_detectionEdgeAlgo(detectionEdgeAlgo)
{
//    std::vector<std::pair<std::string, std::string>> filesToTest{{"test", ".jpg"}, {"test1", ".jpg"}, {"test2", ".jpg"}, {"test3", ".jpg"}, {"test4", ".jpg"}};
//    //m_fileHandler = std::make_shared<FileHandler>();
//    for (const auto &fileName : filesToTest)
//    {
//        const std::string fullFileName = "resources/" + fileName.first + fileName.second;
//        //const auto img = m_fileHandler->loadImage(fullFileName.c_str());
//        //const auto processedImg = processImage(img);
//        const std::string processedFileName = "resources/" + fileName.first + ".png";
//        std::cout << "Storing file: " << processedFileName << std::endl;
//
//        //m_fileHandler->writeImage(processedImg, processedFileName.c_str());
//    }
}

void EdgeDetector::processImage(const std::array<std::array<uint8_t, imgWidth>, imgHeight>& rawImage, std::array<std::array<uint8_t, imgWidth>, imgHeight>& edges)
{
    const size_t sizeW = rawImage.size();
    const size_t sizeH = rawImage.at(0).size();
    const auto totalSize = sizeW * sizeH;

     m_detectionEdgeAlgo->process(rawImage, edges);
}

void sobelGradient(const  uint8_t* image, uint8_t* edges);
int applyKernel(const std::array<std::array<int, 3>, 3> &kernel, const std::array<std::array<int, 3>, 3> &region);

void EdgeDetector::detectEdges(const  uint8_t* image, size_t size)
{
	uint8_t*  edges = new uint8_t[imgWidth * imgHeight];
	sobelGradient(image, edges);

}


void sobelGradient(const  uint8_t* image, uint8_t*  edges)
{
    for (size_t i = 1; i < imgHeight - 1; ++i)
    {
        for (size_t j = 1; j < imgWidth - 1; ++j)
        {
            // Crear una región 3x3 alrededor del píxel actual
            const std::array<std::array<int, 3>, 3> region = {{
                {image[(i - 1) * imgWidth + (j - 1)], image[(i - 1) * imgWidth + j], image[(i - 1) * imgWidth + (j + 1)]},
                {image[i * imgWidth + (j - 1)], image[i * imgWidth + j], image[i * imgWidth + (j + 1)]},
                {image[(i + 1) * imgWidth + (j - 1)], image[(i + 1) * imgWidth + j], image[(i + 1) * imgWidth + (j + 1)]}
            }};
            // Calcular gradientes en dirección x e y
            int gx = applyKernel(sobel_x, region);
            int gy = applyKernel(sobel_y, region);

            // Calcular la magnitud del gradiente
            //edges[i][j] = sqrt(pow(gx, 2) + pow(gy, 2));
            const auto val = sqrt(pow(gx, 2) + pow(gy, 2));
            const auto val2 = std::min(255, static_cast<int>(sqrt(pow(gx, 2) + pow(gy, 2))));
            edges[i * imgWidth + j] = val2;//sqrt(pow(gx, 2) + pow(gy, 2));
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
}
}
