#pragma once
#include "spdlog/spdlog.h"
#include <string>

namespace orbit::logger
{
    spdlog::logger& get_logger();
    void initialize();
}


#ifdef _DEBUG

#define OT_INFO(...) { auto& logger = orbit::logger::get_logger(); logger.info(__VA_ARGS__); }
#define OT_WARN(...) { auto& logger = orbit::logger::get_logger(); logger.warn(__VA_ARGS__); }
#define OT_ERROR(...) { auto& logger = orbit::logger::get_logger(); logger.error(__VA_ARGS__); }
#else
template<typename... targs>
void info(const char* format, targs&&... args){}

template<typename... targs>
void warn(const char* format, targs&&... args){}

template<typename... targs>
void error(const char* format, targs&&... args){}
#endif
