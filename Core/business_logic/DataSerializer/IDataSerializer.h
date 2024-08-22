#pragma once
#include <business_logic/DataSerializer/ISerializableMessage.h>

namespace business_logic
{
namespace DataSerializer
{
class IDataSerializer
{
public:
	virtual void deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data) = 0;
	virtual std::vector<uint8_t> serialize(const ISerializableMessage& msg) const = 0;

};
}
}
