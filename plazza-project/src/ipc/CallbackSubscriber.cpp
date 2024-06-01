#include "ipc/CallbackSubscriber.hpp"

void CallbackSubscriber::handleMessage(std::shared_ptr<IpcMessage> &message) {
    callback(message);
}

CallbackSubscriber::~CallbackSubscriber() = default;

CallbackSubscriber::CallbackSubscriber(
    std::string routingKey,
    const std::function<void(std::shared_ptr<IpcMessage> &message)> &callback)
    : routingKey(std::move(routingKey)), callback(callback) {}

std::string CallbackSubscriber::getRoutingKey() {
    return routingKey;
}
