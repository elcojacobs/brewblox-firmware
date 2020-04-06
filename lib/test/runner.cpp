#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_CONSOLE_WIDTH 300 // workaround for compatiblity with vscode Test Explorer

#include "Logger.h"
#include <catch.hpp>
#include <iostream>

Logger&
logger()
{
    static Logger logger([](Logger::LogLevel level, const std::string& log) {
        std::cerr << "LOG (";
        switch (level) {
        case Logger::LogLevel::DEBUG:
            std::cerr << "DEBUG): ";
            break;
        case Logger::LogLevel::INFO:
            std::cerr << "INFO): ";
            break;
        case Logger::LogLevel::WARN:
            std::cerr << "WARN): ";
            break;
        case Logger::LogLevel::ERROR:
            std::cerr << "ERROR): ";
            break;
        }
        std::cerr << log << std::endl;
    });
    return logger;
}