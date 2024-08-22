#include "DataSerializer.h"

namespace business_logic
{
namespace DataSerializer
{

std::vector<uint8_t> DataSerializer::serialize(const ISerializableMessage& msg) const
{
    return msg.serialize();
}

void DataSerializer::deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data)
{
    msg.deserialize(data);
}
}
}

