#ifndef PLAZZA_PROJECT_KITCHEN_HPP
#define PLAZZA_PROJECT_KITCHEN_HPP

#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include "Cook.hpp"
#include "ipc/IMessageBus.hpp"

class Kitchen {
protected:
    std::string ipcAddress;
    std::shared_ptr<IMessageBus> messageBus;

    std::vector<Cook> cooks;

    std::atomic<bool> _isAvailable;
    const int idleTimeoutInSeconds;
    std::chrono::steady_clock::time_point lastActiveTime;
    std::thread monitoringThread;

public:
    Kitchen(std::string ipcAddress, std::shared_ptr<IMessageBus> messageBus, int numCooks, int idleTimeoutInSeconds);

    ~Kitchen();

    void handleMessage(std::shared_ptr<IpcMessage> &message);

    void runMonitoringThread();

    bool isAvailable(); // true if can accept orders
};

#endif //PLAZZA_PROJECT_KITCHEN_HPP
