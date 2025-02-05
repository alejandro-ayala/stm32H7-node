#pragma once
#include <memory>
#include <sstream>
#include <string>
#include "stm32h7xx_hal.h"
#include "../../hardware_abstraction/Controllers/UART/UARTController.h"
#include "ILogger.h"

namespace services
{

class Logger : public ILogger
{
private:
	Logger();
	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;

	virtual void initialize();

	std::unique_ptr<hardware_abstraction::Controllers::UARTController> outSink;
	LogLevel m_logLevel;
	bool m_disable;

	template<typename... Args>
	std::string concatenateArgsToString(const Args&... args)
	{
	    std::ostringstream oss;
	    ((oss << args << ' '), ...);
	    return oss.str();
	}
public:
	virtual ~Logger() = default;
	static Logger& Instance();
	void setLogLevel(LogLevel) override;
	void disable() override;
	void enable() override;


	void log(LogLevel level , const std::string& msg);

	template<typename... Args>
	void log(LogLevel logLevel , const Args&... args)
	{
		const std::string msg = concatenateArgsToString(args...);
		log(logLevel, msg);
	}
};
} //namespace services
