#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <ostream>

class Logger {
private:
    std::ostream &os;

public:
    explicit Logger(std::ostream &os);

    ~Logger();

    void logInfo(const char *message);

    void logInfo(const std::string &message);

    void logWarning(const char *message);

    void logWarning(const std::string &message);

    void logError(const char *message);

    void logError(const std::string &message);
};

#endif //LOGGER_HPP
