#include "Kitchen.hpp"
#include <iostream>

// Constructor
Kitchen::Kitchen(std::string kitchenIpcAddress, std::string receptionIpcAddress,
                 std::shared_ptr<IMessageBus> messageBus, int numCooks, int replenishTime, int idleTimeoutInSeconds)
    : kitchenIpcAddress(kitchenIpcAddress),
      receptionIpcAddress(receptionIpcAddress),
      messageBus(messageBus),
      _isAvailable(true),
      idleTimeoutInSeconds(idleTimeoutInSeconds),
      lastActiveTime(std::chrono::steady_clock::now()) {
    for (int i = 0; i < numCooks; ++i) {
        cooks.emplace_back();
    }
    monitoringThread = std::thread(&Kitchen::runMonitoringThread, this);
}

// Destructor
Kitchen::~Kitchen() {
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
}

// Monitor the kitchen's idle status
void Kitchen::runMonitoringThread() {
    while (true) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        auto now = std::chrono::steady_clock::now();
        auto idleDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastActiveTime).count();
        if (idleDuration > idleTimeoutInSeconds) {
            _isAvailable = false;
            // Send CloseKitchenMessage to reception
            auto closeMessage = std::make_shared<IpcMessage>(IpcMessageType::CLOSE_KITCHEN, kitchenIpcAddress,
                                                             receptionIpcAddress);
            messageBus->send(closeMessage);
            break;
        }

        auto receivedMessage = messageBus->receive(kitchenIpcAddress);
        if (receivedMessage != nullptr) {
            onMessageReceived(receivedMessage);
        }
    }
}

// Check if kitchen is available
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

// Count available cooks
int Kitchen::countAvailableCooks() const {
    int count = 0;
    for (const auto &cook: cooks) {
        if (!cook.isBusy()) {
            ++count;
        }
    }
    return count;
}

// Dispose kitchen resources
void Kitchen::dispose() {
    // Free resources and cleanup
    std::cout << "Kitchen is closing. Cleaning up resources..." << std::endl;
    // Additional cleanup code if necessary
}

// Get total number of cooks
int Kitchen::getCooksNumber() {
    return cooks.size();
}

// Get replenish time (mock implementation)
int Kitchen::getReplenishTime() {
    return 0; // Assuming some logic for replenishing ingredients
}

void Kitchen::runKitchen() {
    for (int i = 0; i < cooks.size(); i++) {
        if (!cooks[i].isBusy()) {
            cooks[i].startCooking();
        }
    }
}

void Kitchen::onMessageReceived(const std::shared_ptr<IpcMessage>& ipcMessage) {


    if (ipcMessage->getMessageType().getValue() == IpcMessageType::GET_KITCHEN_STATUS) {
        std::shared_ptr<IpcMessage> statusMessage;

        if (_isAvailable) {
            auto kitchenAvailableMessage = std::make_shared<IpcMessage>(
                IpcMessageType::KITCHEN_AVAILABLE, kitchenIpcAddress, receptionIpcAddress);
            return messageBus->send(kitchenAvailableMessage);
        }

        auto kitchenUnavailableMessage = std::make_shared<IpcMessage>(
            IpcMessageType::KITCHEN_UNAVAILABLE, kitchenIpcAddress, receptionIpcAddress);
        return messageBus->send(kitchenUnavailableMessage);
    }
}
