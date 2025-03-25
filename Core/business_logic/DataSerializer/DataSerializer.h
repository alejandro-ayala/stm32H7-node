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
	void deserializeMsg(ISerializableMessage& msg, const std::vector<uint8_t>& data) override;
	void serializeMsg(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const override;
};
}
}
