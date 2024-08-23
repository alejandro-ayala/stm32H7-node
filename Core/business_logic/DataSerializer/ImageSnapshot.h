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
    inline bool operator==(const ImageSnapshot& other)
	{
    	return (m_msgId     == other.m_msgId &&
    			m_imgSize   == other.m_imgSize &&
				m_timestamp == other.m_timestamp);
	}
//private:
    uint8_t  m_msgId;
    uint8_t* m_imgBuffer;
    uint32_t m_imgSize;
    uint32_t m_timestamp;
private:
    friend void to_json(nlohmann::json& j, const ImageSnapshot& image)
    {
        j = nlohmann::json{{"msgId", image.m_msgId}, {"imgSize", image.m_imgSize}};
    }

    friend void from_json(const nlohmann::json& j, ImageSnapshot& image)
    {
        j.at("msgId").get_to(image.m_msgId);
        j.at("imgSize").get_to(image.m_imgSize);
        //j.at("timestamp").get_to(m_timestamp);
    }
};
}
}
