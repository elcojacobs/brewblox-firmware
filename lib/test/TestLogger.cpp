#include "TestLogger.h"
#include "Logger.h"
#include <string>
#include <vector>

std::vector<std::string> logs;

void
TestLogger::clear()
{
    logs.clear();
}

bool
TestLogger::contains(const std::string& s)
{
    for (const auto& log : logs) {
        if (s == log) {
            return true;
        }
    }
    return false;
}

uint32_t
TestLogger::count(const std::string& s)
{
    uint32_t count = 0;
    for (const auto& log : logs) {
        if (s == log) {
            count++;
        }
    }
    return count;
}

void
TestLogger::add(std::string&& s)
{
    logs.push_back(s);
}

Logger&
logger()
{
    static Logger logger([](Logger::LogLevel level, const std::string& log) {
        switch (level) {
        case Logger::LogLevel::DEBUG:
            TestLogger::add("LOG(DEBUG): " + log);
            break;
        case Logger::LogLevel::INFO:
            TestLogger::add("LOG(INFO): " + log);
            break;
        case Logger::LogLevel::WARN:
            TestLogger::add("LOG(WARN): " + log);
            break;
        case Logger::LogLevel::ERROR:
            TestLogger::add("LOG(ERROR): " + log);
            break;
        }
    });
    return logger;
}