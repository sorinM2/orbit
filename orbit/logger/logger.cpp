#include "logger.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <memory>

namespace orbit::logger
{
    namespace
    {
        std::unique_ptr<spdlog::logger> _logger;
    }

    spdlog::logger& get_logger()
    {
        return *_logger.get();
    }

    void initialize()
    {
        auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        auto rotating_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("orbit_logs", 1024*1024, 5, false);

        spdlog::sinks_init_list sink_list = { msvc_sink, rotating_file_sink };
        _logger = std::make_unique<spdlog::logger>("orbit_logger", sink_list);

        _logger->set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
    }
}