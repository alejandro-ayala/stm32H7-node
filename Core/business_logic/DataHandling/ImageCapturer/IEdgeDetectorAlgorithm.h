#pragma once
#include <stddef.h>
#include <cstdint>

namespace business_logic
{
namespace DataHandling
{
class IEdgeDetectorAlgorithm
{
public:
	virtual void process(const uint8_t* rawImage, uint8_t* edges, size_t size) = 0;
};

}
}
