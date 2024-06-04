#include "Logger.hpp"
#include <chrono>

#include "Constants.hpp"
#include "Utils.hpp"

Logger::Logger(std::ostream &os) : os(os) {
}

Logger::~Logger() = default;

void Logger::logDebug(const char *message) {
#if defined(DEBUG) && (DEBUG == 1)
    std::lock_guard<std::mutex> lock(mtx);
    os << "DEBUG\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
#endif
}

void Logger::logDebug(const std::string &message) {
    logDebug(message.c_str());
}

void Logger::logInfo(const char *message) {
    std::lock_guard<std::mutex> lock(mtx);
    os << "INFO\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logInfo(const std::string &message) {
    logInfo(message.c_str());
}

void Logger::logWarning(const char *message) {
    std::lock_guard<std::mutex> lock(mtx);
    os << "WARNING\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logWarning(const std::string &message) {
    logWarning(message.c_str());
}

void Logger::logError(const char *message) {
    std::lock_guard<std::mutex> lock(mtx);
    os << "ERROR\t\t" << timePointToString(std::chrono::system_clock::now()) << "\t\t" << message << '\n';
}

void Logger::logError(const std::string &message) {
    logError(message.c_str());
}
