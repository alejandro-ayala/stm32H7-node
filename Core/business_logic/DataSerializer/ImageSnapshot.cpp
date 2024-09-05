#include "ImageSnapshot.h"
#include "services/Exception/SystemExceptions.h"
#include <vector>
#include "main.h"
namespace business_logic
{
namespace DataSerializer
{
ImageSnapshot::ImageSnapshot(uint8_t msgId, uint8_t msgIndex, uint8_t* msgBuffer, uint32_t imgSize, uint32_t timestamp) : m_msgId(msgId), m_msgIndex(msgIndex), m_imgSize(imgSize), m_timestamp(timestamp)
{
	this->m_imgBuffer = new uint8_t[imgSize];
	BUSINESS_LOGIC_ASSERT( m_imgBuffer != nullptr, services::BusinessLogicErrorId::MemoryAllocationFailed, "Can not allocate memory for image snapshot msg");
	std::copy(msgBuffer + imgSize*msgIndex, (msgBuffer + imgSize*msgIndex) + imgSize, this->m_imgBuffer);
}

ImageSnapshot::~ImageSnapshot()
{
	delete[] this->m_imgBuffer;
}

void ImageSnapshot::serialize(std::vector<uint8_t>& serializeData) const
{
    try
    {
        nlohmann::json j = *this;
        serializeData = nlohmann::json::to_cbor(j);
    }
    catch (const std::exception& e)
    {

    }
}

void ImageSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    nlohmann::json deserialized_json = nlohmann::json::from_cbor(serializeMsg);
    *this = deserialized_json.get<ImageSnapshot>();
}

}
}
