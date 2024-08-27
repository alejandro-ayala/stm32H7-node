#pragma once

#include "ISerializableMessage.h"
#include <iostream>
#include "main.h"

namespace business_logic
{
namespace DataSerializer
{
class ImageSnapshot : public ISerializableMessage
{
public:
	ImageSnapshot() = default;
	ImageSnapshot(uint8_t msgId, uint8_t m_msgIndex, uint8_t* msgBuffer, uint32_t msgSize, uint32_t timestamp);
	~ImageSnapshot();

    void serialize(std::vector<uint8_t>& serializedData) const override;
    void deserialize(const std::vector<uint8_t>& data) override;
    inline bool operator==(const ImageSnapshot& other)
	{
    	return (m_msgId      == other.m_msgId &&
    			m_msgIndex   == other.m_msgIndex &&
    			m_imgSize    == other.m_imgSize &&
				m_timestamp  == other.m_timestamp &&
				std::memcmp(m_imgBuffer, other.m_imgBuffer, MAXIMUN_CBOR_BUFFER_SIZE) == 0);
	}
//private:
    uint8_t  m_msgId;
    uint8_t  m_msgIndex;
    uint8_t* m_imgBuffer;
    uint32_t m_imgSize;
    uint32_t m_timestamp;
private:
    friend void to_json(nlohmann::json& j, const ImageSnapshot& image)
    {
//        std::vector<uint8_t> imgBufferVector(image.m_imgBuffer, image.m_imgBuffer + 4/*image.m_imgSize*/);
//        auto size = imgBufferVector.size();
//        auto freeHeapSize = xPortGetFreeHeapSize();
//        auto minEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
//        j = nlohmann::json{{"msgId", image.m_msgId}, {"imgSize", image.m_imgSize},
//                           {"imgBuffer", imgBufferVector}, {"timestamp", image.m_timestamp}};

        std::cout << "m_imgSize: " << image.m_imgSize << std::endl;
        std::cout << "m_imgBuffer: " << static_cast<void*>(image.m_imgBuffer) << std::endl;

        // Verificación adicional
        if (image.m_imgSize > 0 && image.m_imgBuffer != nullptr)
        {
            try {
                std::vector<uint8_t> imgBufferVector(image.m_imgBuffer + MAXIMUN_CBOR_BUFFER_SIZE*image.m_msgIndex, (image.m_imgBuffer + MAXIMUN_CBOR_BUFFER_SIZE*image.m_msgIndex) + MAXIMUN_CBOR_BUFFER_SIZE);
                j = nlohmann::json{{"msgId", image.m_msgId}, {"imgSize", image.m_imgSize},
                                   {"imgBuffer", imgBufferVector}, {"timestamp", image.m_timestamp}};
            } catch (const std::exception& e) {
                std::cerr << "Error al asignar memoria al vector: " << e.what() << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: m_imgSize es 0 o m_imgBuffer es un puntero nulo." << std::endl;
        }
    }

    friend void from_json(const nlohmann::json& j, ImageSnapshot& image)
    {
    	image.m_msgId = j.at("msgId").get<uint8_t>();
    	image.m_imgSize = j.at("imgSize").get<size_t>();
        std::vector<uint8_t> imgBufferVector = j.at("imgBuffer").get<std::vector<uint8_t>>();
        image.m_imgBuffer = new uint8_t[image.m_imgSize];
        std::copy(imgBufferVector.begin(), imgBufferVector.end(), image.m_imgBuffer);
    	image.m_timestamp = j.at("timestamp").get<uint32_t>();
    }
};
}
}
