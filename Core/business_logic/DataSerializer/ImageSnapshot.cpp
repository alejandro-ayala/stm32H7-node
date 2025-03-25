#include "ImageSnapshot.h"
#include "services/Exception/SystemExceptions.h"
#include "services/Logger/LoggerMacros.h"
#include <vector>

namespace business_logic
{
namespace DataSerializer
{

ImageSnapshot::ImageSnapshot(uint8_t msgId, uint8_t msgIndex, uint8_t* msgBuffer, uint32_t imgSize, uint32_t timestamp) : m_msgId(msgId), m_msgIndex(msgIndex), m_imgSize(imgSize), m_timestamp(timestamp), m_imgBuffer(std::make_unique<uint8_t[]>(imgSize))
{
	BUSINESS_LOGIC_ASSERT( m_imgBuffer != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "Can not allocate memory for image snapshot msg");
	std::copy(msgBuffer, msgBuffer + imgSize, m_imgBuffer.get());
	//TODO comprobar si es mas rapido
	//std::memcpy(m_imgBuffer.get(), msgBuffer, imgSize);
}

ImageSnapshot& ImageSnapshot::operator=(ImageSnapshot&& other) noexcept
{
    if (this != &other) {
        m_msgId = other.m_msgId;
        m_msgIndex = other.m_msgIndex;
        m_imgSize = other.m_imgSize;
        m_timestamp = other.m_timestamp;
        m_imgBuffer = std::move(other.m_imgBuffer);  // Mover el puntero único
    }
    return *this;
}

//void ImageSnapshot::serialize(std::vector<uint8_t>& serializeData) const
//{
//    try
//    {
//        nlohmann::json j = *this;
//        serializeData = nlohmann::json::to_cbor(j);
//    }
//    catch (const std::exception& e)
//    {
//
//    }
//}
void ImageSnapshot::serialize(std::vector<uint8_t>& serializeData) const
{
    try
    {
    	{
        // Creamos el objeto JSON manualmente
        nlohmann::json j;

        j["msgId"] = m_msgId;
        j["msgIndex"] = m_msgIndex;
        j["imgSize"] = m_imgSize;
        j["timestamp"] = m_timestamp;

        // Convertimos el buffer a un vector de bytes
        if (m_imgBuffer && m_imgSize > 0)
        {
            std::vector<uint8_t> imgBufferVector(m_imgBuffer.get(), m_imgBuffer.get() + m_imgSize);
            j["imgBuffer"] = imgBufferVector;
        }
        else
        {
            j["imgBuffer"] = nullptr;
        }

        // Serializamos a CBOR directamente
        serializeData = nlohmann::json::to_cbor(j);
    	}
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error serializando ImageSnapshot a CBOR: {}", e.what());
    }
}

void ImageSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    nlohmann::json deserialized_json = nlohmann::json::from_cbor(serializeMsg);
    // Deserializa los valores manualmente en lugar de usar el operador de asignación
    m_msgId = deserialized_json["msgId"];
    m_msgIndex = deserialized_json["msgIndex"];
    m_imgSize = deserialized_json["imgSize"];
    m_timestamp = deserialized_json["timestamp"];

    m_imgBuffer = std::make_unique<uint8_t[]>(m_imgSize);
    std::memcpy(m_imgBuffer.get(), deserialized_json["imgBuffer"].get<std::vector<uint8_t>>().data(), m_imgSize);
}
}
}
