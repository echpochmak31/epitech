#ifndef KITCHENPOOL_HPP
#define KITCHENPOOL_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include "IPC.hpp"

class KitchenDetails {
public:
    pid_t Pid;
};

class KitchenStatus {
public:
    pid_t Pid;
    std::chrono::steady_clock::time_point statusUpdateTime;
    std::chrono::steady_clock::time_point idleTime;
    bool isAvailable;
    int totalCooks;
    int cooksBusy;
};

class KitchenPool {
private:
    std::vector<pid_t> kitchenPids;

    int numCooks;
    int replenishTime;
    int initailPoolSize;
    std::thread monitoringThread;
    void dispose(pid_t kitchenPid);

public:
    KitchenPool(int numCooks, int replenishTime, int minPoolSize = 2);
    ~KitchenPool();

    KitchenDetails getAvailableKitchen();

    std::vector<KitchenStatus> getStatuses();
};

#endif //KITCHENPOOL_HPP
