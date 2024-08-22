#pragma once

#include "IDataSerializer.h"
#include "ISerializableMessage.h"

namespace business_logic
{
namespace DataSerializer
{
class DataSerializer : public IDataSerializer
{
private:

public:
	DataSerializer() = default;
	virtual ~DataSerializer() = default;
	void deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data) override;
	std::vector<uint8_t> serialize(const ISerializableMessage& msg) const override;
};
}
}
