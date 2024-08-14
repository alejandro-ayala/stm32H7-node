#pragma once
#include "IEdgeDetectorAlgorithm.h"
#include "SobelEdgeDetectorAlgorithm.h"
#include <vector>
#include <array>
#include <memory>
#include "EdgeDetector.h"

namespace business_logic
{
namespace DataHandling
{
class EdgeDetector
{
private:
    std::shared_ptr<SobelEdgeDetectorAlgorithm> m_detectionEdgeAlgo;

public:
    EdgeDetector(const std::shared_ptr<SobelEdgeDetectorAlgorithm> &detectionEdgeAlgo = std::make_shared<SobelEdgeDetectorAlgorithm>());
    ~EdgeDetector() = default;
    void processImage(const std::array<std::array<uint8_t, imgWidth>, imgHeight>& rawImage, std::array<std::array<uint8_t, imgWidth>, imgHeight>& edges);
    void detectEdges(const uint8_t* image, size_t size);
};
}
}
