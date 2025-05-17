#include "Logger.h"
#include "stdio.h"
#include "services/Exception/SystemExceptions.h"

namespace services
{

Logger& Logger::Instance()
{
	static Logger instance;
	return instance;
}

Logger::Logger() : ILogger(), m_logLevel(LogLevel::Info), m_disable(false)
{
	initialize();
}

void Logger::initialize()
{

	UART_HandleTypeDef sinkCff;

	sinkCff.Instance = USART3;
	sinkCff.Init.BaudRate = 115200;
	sinkCff.Init.WordLength = UART_WORDLENGTH_8B;
	sinkCff.Init.StopBits = UART_STOPBITS_1;
	sinkCff.Init.Parity = UART_PARITY_NONE;
	sinkCff.Init.Mode = UART_MODE_TX_RX;
	sinkCff.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	sinkCff.Init.OverSampling = UART_OVERSAMPLING_16;
	sinkCff.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	sinkCff.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	sinkCff.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	int error = 0;
	if (HAL_UART_Init(&sinkCff) != HAL_OK)
	{
		error++;
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&sinkCff, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		error++;
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&sinkCff, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		error++;
	}
	if (HAL_UARTEx_DisableFifoMode(&sinkCff) != HAL_OK)
	{
		error++;
	}
	uartMutex = std::make_shared<business_logic::Osal::MutexHandler>();
	outSink = std::make_unique<hardware_abstraction::Controllers::UARTController>(hardware_abstraction::Controllers::UARTController(&sinkCff));
	outSink->initialize();
}

void Logger::log(LogLevel logLevel, const std::string& msg)
{
	std::string logMsg;
    switch (logLevel)
    {
    case LogLevel::Trace: logMsg = "[TRACE] " + msg; break;
    case LogLevel::Debug: logMsg = "[DEBUG] " + msg; break;
	case LogLevel::Info: logMsg = "[INFO] " + msg; break;
	case LogLevel::Warn: logMsg = "[WARNING] " + msg; break;
	case LogLevel::Error: logMsg = "[ERROR] " + msg; break;
    case LogLevel::Critical: logMsg = "[CRITICAL] " + msg; break;
    default: THROW_SERVICES_EXCEPTION(ServicesErrorId::LoggerUnknownLevel, "Unknown logging level")

    }
    uartMutex->lock();
	outSink->send(logMsg);
	uartMutex->unlock();
}

void Logger::setLogLevel(LogLevel logLevel)
{
	m_logLevel = logLevel;
}

void Logger::disable()
{
	m_disable = true;
}

void Logger::enable()
{
	m_disable = false;
}

bool Logger::isLevelEnabled(LogLevel level) const
{
    return !m_disable && level >= m_logLevel;
}

} //namespace services

