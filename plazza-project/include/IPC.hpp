#ifndef PLAZZA_PROJECT_IPC_HPP
#define PLAZZA_PROJECT_IPC_HPP

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

class IPC {
private:
    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;

public:
    IPC();
    void sendMessage(const std::string& message);
    std::string receiveMessage();
};

#endif //PLAZZA_PROJECT_IPC_HPP
