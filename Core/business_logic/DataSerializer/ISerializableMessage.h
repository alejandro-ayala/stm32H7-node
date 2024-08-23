#pragma once

#include <cstdint>
#include <vector>
//#include "external/cbor-cpp/src/cbor.h"
#include "external/json/include/nlohmann/json.hpp"

namespace business_logic
{
namespace DataSerializer
{
class ISerializableMessage
{
private:

public:
    virtual ~ISerializableMessage() = default;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
};
}
}

