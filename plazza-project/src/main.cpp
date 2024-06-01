#include <iostream>
#include <memory>
#include <thread>

#include "Kitchen.hpp"
#include "ipc/CallbackSubscriber.hpp"
#include "ipc/KernelQueueMessageBus.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"


int main() {
    std::shared_ptr<IMessageBus> bus = std::make_shared<KernelQueueMessageBus>();

    std::string receptionIpcAddress = "reception";
    std::string kitchenIpcAddress = "kitchen1";

    auto onKitchenStatusReceived = [](std::shared_ptr<IpcMessage> &message) {
        if (message->getRoutingKey() != IpcRoutingKeyHolder::GetKitchenStatus) {
            std::cout << "Invalid routing key\n";
            return;
        }
        std::cout << "Kitchen status is: " + message->getMessageType().toString() << std::endl;
    };

    auto onKitchenClosed = [](std::shared_ptr<IpcMessage> &message) {
        std::cout << "Kitchen closed " + message->getIpcAddress() << std::endl;
    };

    auto kitchen = std::make_shared<Kitchen>(kitchenIpcAddress, bus, 2, 4);
    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, onKitchenStatusReceived);
    bus->subscribe(receptionIpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, std::bind(&Kitchen::handleMessage, kitchen,
                                                                           std::placeholders::_1));
    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::CloseKitchen, onKitchenClosed);


    // Start the message handling in a separate thread
    std::thread handlerThread(&IMessageBus::runMessageHandling, bus.get());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "ok\n";

    auto getStatusMessage = std::make_shared<IpcMessage>(IpcMessageType::GET_KITCHEN_STATUS, receptionIpcAddress,
                                                         IpcRoutingKeyHolder::GetKitchenStatus);
    bus->publish(getStatusMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Set the stop flag to true and notify the condition variable
    bus->dispose();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
