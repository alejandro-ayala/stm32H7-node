#include "STM32Timer.h"
#include "services/Logger/LoggerMacros.h"

namespace hardware_abstraction {
namespace Controllers {

STM32Timer::STM32Timer() {
	running = false;
	initialize();
}

void STM32Timer::initialize() {


	  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  htim.Instance = TIM3;
	  htim.Init.Prescaler = 0;
	  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim.Init.Period = 65535;
	  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	  if (HAL_TIM_Base_Init(&htim) != HAL_OK)
	  {
	    ////Error_Handler();
	  }
	  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
	  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
	  if (HAL_TIM_SlaveConfigSynchro(&htim, &sSlaveConfig) != HAL_OK)
	  {
	    //Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
	  {
	    //Error_Handler();
	  }
	  HAL_TIM_Base_MspInit(&htim);

	  timer_freq = HAL_RCC_GetPCLK1Freq();
}

void STM32Timer::startTimer() {
	HAL_TIM_Base_Start(&htim);
	running = true;
}

void STM32Timer::stopTimer() {
	HAL_TIM_Base_Stop(&htim);
	running = false;
}

void STM32Timer::restartTimer() {
	stopTimer();
	__HAL_TIM_SET_COUNTER(&htim, 0);
	startTimer();
}

uint64_t STM32Timer::getCurrentTicks() {
	//auto ticks = __HAL_TIM_GET_COUNTER(&htim);
	TickType_t ticks = xTaskGetTickCount();
	return ticks;
}

double STM32Timer::getCurrentSec() {
	auto currentTicks = getCurrentTicks();
	double seconds = static_cast<double>(currentTicks) / 275e6;
	return seconds;
}

double STM32Timer::getCurrentUsec() {
	auto currentTicks = getCurrentTicks();
	auto currentUs = static_cast<double>(currentTicks) * (1e6 / 275e6);
	return currentUs;
}

double STM32Timer::getCurrentNsec() {
	auto currentTicks = getCurrentTicks();
	auto currentNs = static_cast<double>(currentTicks) * (1e9 / 275e6);

	return currentNs;
}

}
}
