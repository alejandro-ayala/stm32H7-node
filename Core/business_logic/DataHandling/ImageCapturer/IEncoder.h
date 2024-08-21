#pragma once

#include <stdint.h>
#include <cstddef>
#include <vector>

namespace business_logic
{
namespace DataHandling
{

struct RLEFrame
{
	uint8_t pixelValue;
	uint32_t pixelRepetition;
};

class IEncoder
{
public:
	virtual std::vector<uint8_t> decode(const std::vector<uint8_t>& encoded) const = 0;
	virtual void encode(const uint8_t* binarizedImage, const uint32_t imgSize, std::vector<RLEFrame>& encoded) const = 0;
};
}
}
