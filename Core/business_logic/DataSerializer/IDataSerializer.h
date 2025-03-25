#pragma once
#include <business_logic/DataSerializer/ISerializableMessage.h>

namespace business_logic
{
namespace DataSerializer
{
class IDataSerializer
{
public:
	virtual void deserializeMsg(ISerializableMessage& msg, const std::vector<uint8_t>& data) = 0;
	virtual void serializeMsg(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const = 0;

};
}
}
