#include "Reception.hpp"

#include <unistd.h>

std::string Reception::makeKitchenIpcAddress(const pid_t kitchenPid) {
    return "kitchen_pid_" + kitchenPid;
}

void Reception::bindMessageHandlers() {

}

void Reception::createNewKitchen() {

}

void Reception::handleOrder(Order &order) {
}

Reception::Reception(std::shared_ptr<IMessageBus> messageBus, int cooks, int replenish, float multiplier) {
}

void Reception::startHandlingOrders() {
}

