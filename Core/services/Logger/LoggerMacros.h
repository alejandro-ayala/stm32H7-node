#pragma once
#include "services/Logger/Logger.h"

#define LOG_TRACE(...) services::Logger::Instance().log(services::LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...) services::Logger::Instance().log(services::LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...)  services::Logger::Instance().log(services::LogLevel::Info, __VA_ARGS__)
#define LOG_WARNING(...) services::Logger::Instance().log(services::LogLevel::Warn, __VA_ARGS__)
#define LOG_ERROR(...) services::Logger::Instance().log(services::LogLevel::Error, __VA_ARGS__)
#define LOG_FATAL(...) services::Logger::Instance().log(services::LogLevel::Critical, __VA_ARGS__)
