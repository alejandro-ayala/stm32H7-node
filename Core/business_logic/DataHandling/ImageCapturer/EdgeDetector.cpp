#include "EdgeDetector.h"

namespace business_logic
{
namespace DataHandling
{

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

void EdgeDetector::processImage(const  uint8_t* rawImage, uint8_t* edges, size_t size)
{
     m_detectionEdgeAlgo->process(rawImage, edges, size);
}
}
}
