#include "DataSerializer.h"

namespace business_logic
{
namespace DataSerializer
{

void DataSerializer::serializeMsg(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const
{
    msg.serialize(msgSerialized);
}

void DataSerializer::deserializeMsg(ISerializableMessage& msg, const std::vector<uint8_t>& data)
{
    msg.deserialize(data);
}
}
}

