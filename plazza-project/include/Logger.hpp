#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <condition_variable>
#include <mutex>
#include <ostream>

class Logger {
private:
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::ostream &os;

public:
    explicit Logger(std::ostream &os);

    ~Logger();

    void logDebug(const char *message);

    void logDebug(const std::string &message);

    void logInfo(const char *message);

    void logInfo(const std::string &message);

    void logWarning(const char *message);

    void logWarning(const std::string &message);

    void logError(const char *message);

    void logError(const std::string &message);
};

#endif //LOGGER_HPP
