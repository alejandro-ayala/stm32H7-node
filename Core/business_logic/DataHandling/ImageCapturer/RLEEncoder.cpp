
#include "RLEEncoder.h"
#include "services/Exception/SystemExceptions.h"
#include <array>
namespace business_logic
{
namespace DataHandling
{

std::vector<uint8_t> RLEEncoder::decode(const std::vector<uint8_t>& encoded) const
{
	std::vector<uint8_t> decoded;

	if (encoded.size() % 2 != 0)
	{
		THROW_BUSINESS_LOGIC_EXCEPTION(services::BusinessLogicErrorId::InvalidArgument, "Invalid coded vector size");
	}

	for (size_t i = 0; i < encoded.size(); i += 2)
	{
		uint8_t value = encoded[i];
		uint8_t count = encoded[i + 1];

		for (uint8_t j = 0; j < count; ++j)
		{
			decoded.push_back(value);
		}
	}

	return decoded;

}

void RLEEncoder::encode(const uint8_t* binarizedImage, const uint32_t imgSize, std::vector<RLEFrame>& encoded) const
{

    uint32_t count = 1;
    encoded.clear();
    for (uint32_t i = 1; i < imgSize; ++i)
    {
    	if(binarizedImage[i] == 0x00 || binarizedImage[i] == 0xFF)
    	{
            if (binarizedImage[i] == binarizedImage[i - 1])
            {
                count++;
            }
            else
            {
            	auto idx = encoded.size();
            	RLEFrame frame;
            	frame.pixelValue = binarizedImage[i - 1];
            	frame.pixelRepetition = count;
                encoded.push_back(frame);
                count = 1;
            }
    	}

    }
}
}
}
