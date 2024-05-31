#include "IPC.hpp"

IPC::IPC() : pipefd{0, 0} {
    if (pipe(pipefd) == -1) {
        throw std::runtime_error("Failed to create pipe");
    }
}

IPC::~IPC() {
    close(pipefd[0]);
    close(pipefd[1]);
}

int IPC::getReadFd() const {
    return pipefd[0];
}

int IPC::getWriteFd() const {
    return pipefd[1];
}

IPC& IPC::operator<<(const std::string& data) {
    if (write(pipefd[1], data.c_str(), data.size()) == -1) {
        throw std::runtime_error("Failed to write to pipe");
    }
    return *this;
}

IPC& IPC::operator>>(std::string& data) {
    char buffer[256];
    ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        throw std::runtime_error("Failed to read from pipe");
    }
    buffer[bytesRead] = '\0';
    data = buffer;
    return *this;
}
