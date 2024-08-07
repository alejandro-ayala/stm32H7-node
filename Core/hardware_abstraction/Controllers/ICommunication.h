
#pragma once
#include <string>
#include "Definitions.h"
#include "IController.h"

namespace hardware_abstraction
{
namespace Controllers
{
class ICommunication : public IController {
public:
	ICommunication(cstring name) : IController(name) {};
	virtual ~ICommunication(){};

	virtual void send(const std::string&) = 0;
	virtual void receive(std::string&, uint32_t waitResponse = 2000)    = 0;
};
}
}
