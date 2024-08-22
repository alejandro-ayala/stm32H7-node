#pragma once

#include <cstdint>
#include <vector>
#include "external/cbor-cpp/src/cbor.h"

namespace business_logic
{
namespace DataSerializer
{
class ISerializableMessage
{
private:
	cbor::output_dynamic m_outputStream;
	cbor::listener_debug m_listener;
public:
    virtual ~ISerializableMessage() = default;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
};
}
}

