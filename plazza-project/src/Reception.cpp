#include "Reception.hpp"

#include <unistd.h>

std::string Reception::makeKitchenIpcAddress(const pid_t kitchenPid) {
    return "kitchen_pid_" + kitchenPid;
}

void Reception::bindMessageHandlers() {

}

void Reception::createNewKitchen() {

}


Reception::Reception(std::shared_ptr<IMessageBus> messageBus, int cooks, int replenish, float multiplier) {
}

void Reception::startHandlingOrders() {
}

