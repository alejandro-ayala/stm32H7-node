#include "Logger.h"
#include "stdio.h"
#include <cstring>
#include "services/Exception/SystemExceptions.h"

namespace services
{

Logger& Logger::Instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : ILogger(), m_logLevel(LogLevel::Info), m_disable(false), outSink(nullptr)
{
    initialize();
}

void Logger::initialize()
{
    static UART_HandleTypeDef sinkCff;

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
    //static business_logic::Osal::MutexHandler staticMutex;
    static hardware_abstraction::Controllers::UARTController staticUartController(&sinkCff);
    ////uartMutex = &staticMutex;
    uartMutex = xSemaphoreCreateMutex();
    outSink = &staticUartController;
    outSink->initialize();
}

void Logger::log(LogLevel logLevel, const char* msg)
{
    static char logMsg[600];
    const char* prefix = "";
    switch (logLevel)
    {
    case LogLevel::Trace:    prefix = "[TRACE] "; break;
    case LogLevel::Debug:    prefix = "[DEBUG] "; break;
    case LogLevel::Info:     prefix = "[INFO] "; break;
    case LogLevel::Warn:     prefix = "[WARNING] "; break;
    case LogLevel::Error:    prefix = "[ERROR] "; break;
    case LogLevel::Critical: prefix = "[CRITICAL] "; break;
    default: THROW_SERVICES_EXCEPTION(ServicesErrorId::LoggerUnknownLevel, "Unknown logging level")
    }
    std::snprintf(logMsg, sizeof(logMsg), "%s%s", prefix, msg);
    xSemaphoreTake(uartMutex, portMAX_DELAY);
    outSink->send(logMsg, strlen(logMsg));
    xSemaphoreGive(uartMutex);
}

void Logger::unsafeLog(LogLevel level, const char* msg)
{
    static char logMsg[600];
    const char* prefix = "";
    switch (level)
    {
    case LogLevel::Trace:    prefix = "[TRACE] "; break;
    case LogLevel::Debug:    prefix = "[DEBUG] "; break;
    case LogLevel::Info:     prefix = "[INFO] "; break;
    case LogLevel::Warn:     prefix = "[WARNING] "; break;
    case LogLevel::Error:    prefix = "[ERROR] "; break;
    case LogLevel::Critical: prefix = "[CRITICAL] "; break;
    default: prefix = "[UNKNOWN] "; break;
    }
    std::snprintf(logMsg, sizeof(logMsg), "%s%s", prefix, msg);
    outSink->send(logMsg, strlen(logMsg));
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
