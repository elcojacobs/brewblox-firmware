#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_CONSOLE_WIDTH 300 // workaround for compatiblity with vscode Test Explorer

#include "Logger.h"
#include "TestLogger.h"
#include <catch.hpp>

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