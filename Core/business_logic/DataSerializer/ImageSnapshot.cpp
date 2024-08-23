#include "ImageSnapshot.h"
#include <vector>

namespace business_logic
{
namespace DataSerializer
{
std::vector<uint8_t> ImageSnapshot::serialize() const
{

    nlohmann::json j = *this;
    std::vector<uint8_t> serializeData = nlohmann::json::to_cbor(j);
	return serializeData;
}

void ImageSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    nlohmann::json deserialized_json = nlohmann::json::from_cbor(serializeMsg);
    *this = deserialized_json.get<ImageSnapshot>();
}

}
}
