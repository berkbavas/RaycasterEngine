
#include "Logger.h"

void RaycasterEngine::Logger::Log(LogLevel level, const std::string& message)
{
    thread_local const auto TL_THREAD_ID = mLastThreadId.fetch_add(1);

    const auto log = std::format("{:.12} [{:}] [ {:5} ] {:}", GetTimeString(), TL_THREAD_ID, GetLogLevelString(level), message);

    switch (level)
    {
        case LogLevel::TRACE:
        case LogLevel::DEBUG:

        case LogLevel::INFO:
            std::cout << log << std::endl;
            break;
        case LogLevel::WARNING:
        case LogLevel::FATAL:
            std::cerr << log << std::endl;
            break;
        default:
            break;
    }
}

void RaycasterEngine::Logger::SetLogLevel(LogLevel logLevel)
{
    mLogLevel = logLevel;
}

RaycasterEngine::LogLevel RaycasterEngine::Logger::GetLogLevel()
{
    return mLogLevel;
}

std::string RaycasterEngine::Logger::GetTimeString()
{
    const auto zone = std::chrono::current_zone();
    const auto now = std::chrono::system_clock::now();
    const auto local = std::chrono::zoned_time(zone, now);

    return std::format("{:%T}", local);
}

bool RaycasterEngine::Logger::isLogEnabledFor(LogLevel level)
{
    return mLogLevel <= level;
}

RaycasterEngine::LogLevel RaycasterEngine::Logger::mLogLevel = RaycasterEngine::LogLevel::ALL;

std::atomic_uint32_t RaycasterEngine::Logger::mLastThreadId = 0;