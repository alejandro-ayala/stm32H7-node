#pragma once
#include "ITimer.h"
#include "stm32h7xx_hal.h"

namespace hardware_abstraction {
namespace Controllers {

class STM32Timer : public ITimer {
private:
    TIM_HandleTypeDef htim;
    uint32_t timer_freq;
    uint32_t prescaler;
    uint32_t period;

public:
    STM32Timer();
    ~STM32Timer() = default;

    void initialize();

    void startTimer() override;
    void stopTimer() override;
    void restartTimer() override;
    uint64_t getCurrentTicks() override;
    double getCurrentSec() override;
    double getCurrentUsec() override;
    double getCurrentNsec() override;
};

}
}
