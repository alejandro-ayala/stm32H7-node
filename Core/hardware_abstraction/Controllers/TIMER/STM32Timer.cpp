#include "STM32Timer.h"
#include "services/Logger/LoggerMacros.h"

TIM_HandleTypeDef htim;
uint64_t elapsedTimeCnt;
static uint32_t start = 0;
static uint32_t stop = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	const std::string logMsg = "HAL_TIM_PeriodElapsedCallback -- elapsedTimeCnt: " + std::to_string(elapsedTimeCnt);
	UNSAFELOG_INFO(logMsg.c_str());

    if (htim->Instance == TIM5)
    {
        stop = HAL_GetTick();  // Devuelve milisegundos desde el boot
        uint32_t duration_ms = stop - start;
        start = stop;
    	const std::string logMsg = "Timer overflow: " + std::to_string(duration_ms) + " ms";

        UNSAFELOG_INFO(logMsg.c_str());
    	elapsedTimeCnt++;
    }
}

namespace hardware_abstraction {
namespace Controllers {

STM32Timer::STM32Timer() {
	running = false;
	initialize();
}

void STM32Timer::initialize() {


	  /* USER CODE BEGIN TIM5_Init 0 */

	  /* USER CODE END TIM5_Init 0 */

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  /* USER CODE BEGIN TIM5_Init 1 */

	  /* USER CODE END TIM5_Init 1 */
	  htim.Instance = TIM5;
	  htim.Init.Prescaler = 0;
	  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	  htim.Init.Period = 4294967295;
	  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(&htim) != HAL_OK)
	  {
	    ////Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
	  {
	    //Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig) != HAL_OK)
	  {
	    //Error_Handler();
	  }


	  timer_freq = HAL_RCC_GetPCLK1Freq();
	  LOG_INFO("STM32Timer::initialize timer_freq: ", std::to_string(timer_freq));
}

void STM32Timer::startTimer() {
	//HAL_TIM_Base_Start(&htim);
    HAL_TIM_Base_Start_IT(&htim);
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
	auto ticks = __HAL_TIM_GET_COUNTER(&htim);
	//TickType_t ticks = xTaskGetTickCount();
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
    uint64_t currentTicks = getCurrentTicks();

    constexpr uint64_t ticksPerOverflow = 0xFFFFFFFFULL + 1ULL;

    uint64_t totalTicks = (elapsedTimeCnt * ticksPerOverflow) + currentTicks;

    constexpr double tickDurationNs = 1e9 / 275e6;

    double currentNs = static_cast<double>(totalTicks) * tickDurationNs;
    return currentNs;
}

uint64_t STM32Timer::getElapsedTimeCounter() const
{
	return elapsedTimeCnt;
}

}
}
