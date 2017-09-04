#include <mutex>
#include <vector>
#include "Log.h"

static bool init = false;
static std::mutex mtx;
std::shared_ptr<spdlog::logger> logger;

void initLogger(bool hasStdOut, int fileSize, int numFiles)
{
    if (!init)
    {
        std::lock_guard<std::mutex> lg(mtx);
        if (!init)
        {
            init = true;
            
            std::vector<spdlog::sink_ptr> sinks;
            if (hasStdOut)
                sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logfile", fileSize, numFiles));
            logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
            spdlog::register_logger(logger);
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");
        }
    }
}