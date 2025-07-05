#include "STM32Timer.h"
#include "services/Logger/LoggerMacros.h"

namespace hardware_abstraction {
namespace Controllers {

STM32Timer::STM32Timer() {
	running = false;
	initialize();
}

void STM32Timer::initialize() {


	  /* USER CODE BEGIN TIM2_Init 0 */

	  /* USER CODE END TIM2_Init 0 */

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  /* USER CODE BEGIN TIM2_Init 1 */

	  /* USER CODE END TIM2_Init 1 */
	  htim2.Instance = TIM2;
	  htim2.Init.Prescaler = 0;
	  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim2.Init.Period = 0xFFFFFFFF ;
	  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* USER CODE BEGIN TIM2_Init 2 */

	  /* USER CODE END TIM2_Init 2 */

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
