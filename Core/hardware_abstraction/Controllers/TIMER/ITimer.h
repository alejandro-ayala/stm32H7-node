#pragma once
#include <stdint.h>

namespace hardware_abstraction {
namespace Controllers {

class ITimer {
public:
	bool    running;

	ITimer() {};
	virtual ~ITimer(){};

	virtual void startTimer()    = 0;
	virtual void stopTimer()     = 0;
	virtual void restartTimer()  = 0;
	virtual uint64_t getCurrentTicks() = 0;
	virtual uint64_t getElapsedTimeCounter() const = 0;
	virtual double getCurrentSec()   = 0;
	virtual double getCurrentUsec()  = 0;
	virtual double getCurrentNsec()  = 0;
};
}
}
