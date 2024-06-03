#include "Logger.hpp"
#include <chrono>
#include "Utils.hpp"

Logger::Logger(std::ostream &os) : os(os) {
}

Logger::~Logger() = default;

void Logger::logInfo(const char *message) {
    os << "INFO\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logInfo(const std::string &message) {
    logInfo(message.c_str());
}

void Logger::logWarning(const char *message) {
    os << "WARNING\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logWarning(const std::string &message) {
    logWarning(message.c_str());
}

void Logger::logError(const char *message) {
    os << "ERROR\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logError(const std::string &message) {
    logError(message.c_str());
}
