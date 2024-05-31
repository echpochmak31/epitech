#ifndef PLAZZA_PROJECT_IPC_HPP
#define PLAZZA_PROJECT_IPC_HPP

#include <unistd.h>
#include <stdexcept>
#include <string>

class IPC {
private:
    int pipefd[2]; // 0: read end, 1: write end

public:
    IPC();
    ~IPC();
    int getReadFd() const;
    int getWriteFd() const;
    IPC& operator<<(const std::string& data);
    IPC& operator>>(std::string& data);
};

#endif //PLAZZA_PROJECT_IPC_HPP
