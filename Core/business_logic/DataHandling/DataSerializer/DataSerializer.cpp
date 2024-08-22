#include "DataSerializer.h"

namespace business_logic
{
namespace DataHandling
{

//DataSerializer::DataSerializer()
//{
//	//m_listener = std::make_shared<cbor::listener_debug>();
//}

void DataSerializer::serialize()
{
    cbor::encoder encoder(m_outputStream);
    encoder.write_array(5);
    {
        encoder.write_int(123);
        encoder.write_string("bar");
        encoder.write_int(321);
        encoder.write_int(321);
        encoder.write_string("foo");
    }
}

void DataSerializer::deserialize()
{
    cbor::input input(m_outputStream.data(), m_outputStream.size());

    cbor::decoder decoder(input, m_listener);
    decoder.run();
}
}
}
