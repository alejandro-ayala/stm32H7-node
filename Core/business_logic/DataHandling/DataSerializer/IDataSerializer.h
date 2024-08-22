#pragma once

namespace business_logic
{
namespace DataHandling
{
class IDataSerializer
{
public:
	virtual void deserialize() = 0;
	virtual void serialize() = 0;

};
}
}
