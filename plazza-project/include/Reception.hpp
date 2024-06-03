#ifndef PLAZZA_PROJECT_RECEPTION_HPP
#define PLAZZA_PROJECT_RECEPTION_HPP

#include <memory>
#include <vector>
#include <string>

#include "ipc/IMessageBus.hpp"

class Reception {
private:
    std::shared_ptr<IMessageBus> messageBus;
    std::vector<pid_t> kitchenPIDs;
    int numCooks;
    int replenishTime;
    float cookTimeMultiplier;

    static std::string makeKitchenIpcAddress(pid_t kitchenPid);
    void bindMessageHandlers();
    void createNewKitchen();
public:
    Reception(std::shared_ptr<IMessageBus> messageBus, int cooks, int replenish, float multiplier);

    void startHandlingOrders();
};

#endif //PLAZZA_PROJECT_RECEPTION_HPP
