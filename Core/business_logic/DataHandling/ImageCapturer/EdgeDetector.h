#pragma once
#include "IEdgeDetectorAlgorithm.h"
#include "SobelEdgeDetectorAlgorithm.h"
#include <vector>
#include <array>
#include <memory>

namespace business_logic
{
namespace DataHandling
{
class EdgeDetector
{
private:
    std::shared_ptr<SobelEdgeDetectorAlgorithm> m_detectionEdgeAlgo;

public:
    EdgeDetector(const std::shared_ptr<SobelEdgeDetectorAlgorithm> &detectionEdgeAlgo);
    ~EdgeDetector() = default;
    void processImage(const uint8_t* rawImage, uint8_t* edges, size_t size);
};
}
}
