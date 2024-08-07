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

Logger::Logger() : ILogger(), m_logLevel(LogLevel::Debug), m_disable(false)
{
	initialize();
}

void Logger::initialize()
{

	UART_HandleTypeDef sinkCff;
	sinkCff.Instance          = USART1;
	sinkCff.Init.BaudRate     = 115200;
	sinkCff.Init.WordLength   = UART_WORDLENGTH_8B;
	sinkCff.Init.StopBits     = UART_STOPBITS_1;
	sinkCff.Init.Parity       = UART_PARITY_NONE;
	sinkCff.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	sinkCff.Init.Mode         = UART_MODE_TX_RX;
	sinkCff.Init.OverSampling = UART_OVERSAMPLING_16;

	outSink = std::make_unique<hardware_abstraction::Controllers::UARTController>(hardware_abstraction::Controllers::UARTController(&sinkCff));
	outSink->initialize();
}

void Logger::log(LogLevel logLevel, const std::string& msg)
{
	if((m_logLevel > logLevel) || m_disable) return;

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
	outSink->send(logMsg);
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

} //namespace services

