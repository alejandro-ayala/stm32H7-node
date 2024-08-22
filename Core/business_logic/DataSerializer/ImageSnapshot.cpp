#include "ImageSnapshot.h"
#include <vector>

namespace business_logic
{
namespace DataSerializer
{
std::vector<uint8_t> ImageSnapshot::serialize() const
{

	cbor::output_dynamic output;
    cbor::encoder encoder(output);
    encoder.write_array(5);
	encoder.write_int(123);
	encoder.write_string("bar");
	encoder.write_int(321);
	encoder.write_int(321);
	encoder.write_string("foo");

    std::vector<uint8_t> serializeData;
    auto msgPtr = output.data();
    for(uint32_t i = 0; i < output.size() ; i++)
    {
    	serializeData.push_back(msgPtr[i]);
    }
	return serializeData;
}

void ImageSnapshot::deserialize(const std::vector<uint8_t>& serializeMsg)
{
    cbor::input input(const_cast<uint8_t*>(serializeMsg.data()), serializeMsg.size());
    cbor::listener_debug listener;
    cbor::decoder decoder(input, listener);
    decoder.run();
}

}
}
