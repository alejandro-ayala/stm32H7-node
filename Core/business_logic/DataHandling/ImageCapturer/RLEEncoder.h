#pragma once

#include "IEncoder.h"

namespace business_logic
{
namespace DataHandling
{
class RLEEncoder : public IEncoder
{
public:
	RLEEncoder() = default;
	~RLEEncoder() = default;
	std::vector<uint8_t> decode(const std::vector<uint8_t>& encoded) const override;
	void encode(const uint8_t* binarized_image, const uint32_t imgSize, std::vector<RLEFrame>& encoded) const override;
};
}
}
