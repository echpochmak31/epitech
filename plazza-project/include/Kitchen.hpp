#ifndef PLAZZA_PROJECT_KITCHEN_HPP
#define PLAZZA_PROJECT_KITCHEN_HPP

#include <atomic>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "Logger.hpp"
#include "ThreadSafeQueue.hpp"
#include "ipc/IMessageBus.hpp"
#include "ipc/Payloads.hpp"

class Kitchen {
protected:
    std::shared_ptr<Logger> logger;
    std::string ipcAddress;
    std::shared_ptr<IMessageBus> messageBus;

    const int numCooks;
    const float cookingTimeMultiplier;

    ThreadSafeQueue<OrderedPizzaDto> orderedPizzas;
    ThreadSafeQueue<OrderedPizzaDto> cookedPizzas;
    std::thread queuesHandlingThread;

    std::mutex mtx;
    std::condition_variable cv;
    std::vector<bool> threadIsFree;
    std::vector<std::thread> cookThreads;

    std::atomic<bool> isClosing{};
    std::chrono::steady_clock::time_point lastActiveTime;
    std::thread monitoringThread;

    int getAvailableCookNumber() const;

    void cookPizza(OrderedPizzaDto orderedPizzaDto, int cookIndex);

    void handleAcceptOrderedPizzaRequest(std::shared_ptr<IpcMessage> &message);


    void assignOrderedPizza(OrderedPizzaDto orderedPizzaDto);

    void handleGetStatusRequest(std::shared_ptr<IpcMessage> &message);

public:
    Kitchen(std::shared_ptr<Logger> logger, std::string ipcAddress, std::shared_ptr<IMessageBus> messageBus,
            int numCooks, float cookingTimeMultiplier);

    ~Kitchen();

    void monitorKitchenStatus(); // must run in another thrad
    void handleQueues(); // must run in another thrad

    void handleMessage(std::shared_ptr<IpcMessage> &message);
};

#endif //PLAZZA_PROJECT_KITCHEN_HPP
