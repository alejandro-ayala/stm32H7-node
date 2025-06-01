#pragma once
#include <string>
#include "stm32h7xx_hal.h"
#include "../../hardware_abstraction/Controllers/UART/UARTController.h"
#include "ILogger.h"
#include "business_logic/Osal/MutexHandler.h"
#include <cstdio>
#include <type_traits>

namespace services
{

class Logger : public ILogger
{
private:
    Logger();
    Logger(const Logger&) = delete;
    void operator=(const Logger&) = delete;

    virtual void initialize();

    hardware_abstraction::Controllers::UARTController* outSink;
    LogLevel m_logLevel;
    bool m_disable;
    SemaphoreHandle_t uartMutex;

    // Conversión segura a const char*
    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
    to_cstr(const T& arg) { return std::to_string(arg); }
    inline const char* to_cstr(const std::string& arg) { return arg.c_str(); }
    inline const char* to_cstr(const char* arg) { return arg; }
    inline const char* to_cstr(char* arg) { return arg; }
    template<size_t N>
    inline const char* to_cstr(const char (&arg)[N]) { return arg; }

    // Concatenación en buffer estático
    template<typename... Args>
    const char* concatenateArgsToString(const Args&... args)
    {
        static char buffer[512];
        size_t written = 0;
        auto append = [&](const auto& arg) {
            auto str = to_cstr(arg);
            written += std::snprintf(buffer + written, sizeof(buffer) - written, "%s ", str);
        };
        (append(args), ...);
        if (written > 0 && buffer[written - 1] == ' ') buffer[written - 1] = '\0';
        else buffer[written] = '\0';
        return buffer;
    }

public:
    virtual ~Logger() = default;
    static Logger& Instance();
    void setLogLevel(LogLevel) override;
    void disable() override;
    void enable() override;
    bool isLevelEnabled(LogLevel level) const;

    void log(LogLevel level, const char* msg);

    template<typename... Args>
    void log(LogLevel logLevel, const Args&... args)
    {
        if((m_logLevel > logLevel) || m_disable)
        {
            return;
        }
        else
        {
            const char* msg = concatenateArgsToString(args...);
            log(logLevel, msg);
        }
    }
};
} //namespace services
