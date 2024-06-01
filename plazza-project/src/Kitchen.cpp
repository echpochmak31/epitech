#include "Kitchen.hpp"
#include <iostream>

#include "ipc/IpcRoutingKeyHolder.hpp"

Kitchen::Kitchen(std::string ipcAddress, std::shared_ptr<IMessageBus> messageBus, int numCooks,
                 int idleTimeoutInSeconds)
    : ipcAddress(ipcAddress),
      messageBus(messageBus),
      _isAvailable(true),
      idleTimeoutInSeconds(idleTimeoutInSeconds),
      lastActiveTime(std::chrono::steady_clock::now()) {
    for (int i = 0; i < numCooks; ++i) {
        cooks.emplace_back();
    }
    // todo
    monitoringThread = std::thread(&Kitchen::runMonitoringThread, this);
}


Kitchen::~Kitchen() {
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
}


void Kitchen::handleMessage(std::shared_ptr<IpcMessage> &message) {
    if (message->getMessageType().getValue() == IpcMessageType::GET_KITCHEN_STATUS) {
        std::shared_ptr<IpcMessage> statusMessage;

        if (_isAvailable) {
            const auto responseMessage = std::make_shared<IpcMessage>(
                IpcMessageType::KITCHEN_AVAILABLE, ipcAddress, IpcRoutingKeyHolder::GetKitchenStatus);

            return messageBus->publish(responseMessage);
        }

        const auto responseMessage = std::make_shared<IpcMessage>(
            IpcMessageType::KITCHEN_UNAVAILABLE, ipcAddress, IpcRoutingKeyHolder::GetKitchenStatus);

        return messageBus->publish(responseMessage);
    }
}

bool Kitchen::isAvailable() {
    for (const auto &cook: cooks) {
        if (!cook.isBusy()) {
            return true;
        }
    }
    lastActiveTime = std::chrono::steady_clock::now();
    _isAvailable = false;
    return false;
}


// Monitor the kitchen's idle status
void Kitchen::runMonitoringThread() {
    std::cout << "Kitchen monitoring thread started" << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // todo: remove hardcode
        auto now = std::chrono::steady_clock::now();
        auto idleDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastActiveTime).count();
        if (idleDuration > idleTimeoutInSeconds) {
            _isAvailable = false;
            auto message = std::make_shared<IpcMessage>(IpcMessageType::CLOSE_KITCHEN, ipcAddress,
                                                        IpcRoutingKeyHolder::CloseKitchen);
            messageBus->publish(message);
            std::cout << "Kitchen sent CLOSE_KITCHEN message" << std::endl;
            break;
        }
    }
    std::cout << "Kitchen monitoring thread exiting" << std::endl;
}
