#ifndef PLAZZA_PROJECT_KITCHEN_HPP
#define PLAZZA_PROJECT_KITCHEN_HPP

#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include "Cook.hpp"
#include "Pizza.hpp"
#include "ipc/IMessageBus.hpp"

class Kitchen {
private:
    std::string kitchenIpcAddress;
    std::string receptionIpcAddress;
    std::shared_ptr<IMessageBus> messageBus;
    std::vector<Cook> cooks;
    // IngredientStock stock;

    std::atomic<bool> _isAvailable;
    const int idleTimeoutInSeconds;
    std::chrono::steady_clock::time_point lastActiveTime;
    std::thread monitoringThread;

    // std::thread replenishThread;
    void dispose();
    void onMessageReceived(const std::shared_ptr<IpcMessage>& ipcMessage);
public:
    Kitchen(std::string kitchenIpcAddress, std::string receptionIpcAddress, std::shared_ptr<IMessageBus> messageBus, int numCooks, int replenishTime, int idleTimeoutInSeconds);
    ~Kitchen();

    void onPizzaIsReady(Cook& cook, Pizza& pizza);
    void runMonitoringThread();
    void runKitchen();
    bool isAvailable(); // true if can accept orders

    int countAvailableCooks() const;
    int getCooksNumber();
    int getReplenishTime();
};

#endif //PLAZZA_PROJECT_KITCHEN_HPP
