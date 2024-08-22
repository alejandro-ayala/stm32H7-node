#pragma once

#include "IDataSerializer.h"
#include "external/cbor-cpp/src/cbor.h"

namespace business_logic
{
namespace DataHandling
{
class DataSerializer : public IDataSerializer
{
private:

	cbor::output_dynamic m_outputStream;
	cbor::listener_debug m_listener;
public:
	DataSerializer() = default;
	virtual ~DataSerializer() = default;
	void deserialize() override;
	void serialize() override;
};
}
}
