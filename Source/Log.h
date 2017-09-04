#pragma once

#include <memory>
#include "spdlog/spdlog.h"

void initLogger(bool hasStdOut = true, int fileSize = 16 * 1024 *1024, int numFiles = 4);

extern std::shared_ptr<spdlog::logger> logger;

#if 0

#define LOG_STR_H(x) #x
#define LOG_STR_HELPER(x) LOG_STR_H(x)

#ifdef _WIN32

#define LOG_TRACE(...) logger->trace("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_DEBUG(...) logger->debug("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_INFO(...) logger->info("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_WARNING(...) logger->warn("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_ERROR(...) logger->error("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_CRITICAL(...) logger->critical("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_FATAL(...) logger->critical("[" __FILE__ " " __FUNCTION__ " line #" LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)

#else

#define LOG_TRACE(...) logger->trace("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_DEBUG(...) logger->debug("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_INFO(...) logger->info("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_WARNING(...) logger->warn("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_ERROR(...) logger->error("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_CRITICAL(...) logger->critical("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)
#define LOG_FATAL(...) logger->critical("[" __FILE__ " " LOG_STR_HELPER(__LINE__) "] " __VA_ARGS__)

#endif

#else

#include <string.h>
inline const char* shortFileName(const char* fileName)
{
#ifdef _WIN32
    const char* ptr = strrchr(fileName, '\\');
#else
    const char* ptr = strrchr(fileName, '/');
#endif
    return ptr ? ptr + 1 : fileName;
}

// I have to use ##__VAR_ARGS__ instead of __VAR_ARGS__, see
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

#if 0

#define LOG_TRACE(msg, ...) logger->trace("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) logger->debug("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) logger->info("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) logger->warn("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) logger->error("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) logger->critical("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) logger->critical("[{} {}] " msg, shortFileName(__FILE__), __LINE__, ##__VA_ARGS__)

#else

// https://stackoverflow.com/questions/4384765/whats-the-difference-between-pretty-function-function-func
#ifdef _WIN32
//#define __FUNCTION_NAME__ __FUNCSIG__
#define __FUNCTION_NAME__ __FUNCTION__
#else
//#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#define LOG_TRACE(msg, ...) logger->trace("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) logger->debug("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) logger->info("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_WARNING(msg, ...) logger->warn("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) logger->error("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) logger->critical("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) logger->critical("[{} {} {}] " msg, shortFileName(__FILE__), __LINE__, __FUNCTION_NAME__, ##__VA_ARGS__)

#endif

#endif
