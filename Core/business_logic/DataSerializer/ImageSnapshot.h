#pragma once

#include "ISerializableMessage.h"

namespace business_logic
{
namespace DataSerializer
{
class ImageSnapshot : public ISerializableMessage
{
public:

    std::vector<uint8_t> serialize() const override;
    void deserialize(const std::vector<uint8_t>& data) override;
//private:
    uint8_t  m_msgId;
    uint8_t* m_imgBuffer;
    uint32_t m_imgSize;
    uint32_t m_timestamp;
};
}
}
