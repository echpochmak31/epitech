#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "Kitchen.hpp"
#include "ipc/CallbackSubscriber.hpp"
#include "ipc/KernelQueueMessageBus.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"
#include "ipc/Payloads.hpp"

std::string timePointToString(const std::chrono::system_clock::time_point &tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int main() {
    std::shared_ptr<IMessageBus> bus = std::make_shared<KernelQueueMessageBus>();

    std::string receptionIpcAddress = "reception";
    std::string kitchenIpcAddress = "kitchen1";

    auto onKitchenStatusReceived = [](std::shared_ptr<IpcMessage> &message) {
        if (message->getRoutingKey() != IpcRoutingKeyHolder::GetKitchenStatus) {
            std::cout << "Invalid routing key\n";
            return;
        }
        if (message->getMessageType().getValue() != IpcMessageType::GET_KITCHEN_STATUS_RESPONSE) {
            std::cout << "Invalid message type\n";
            return;
        }
        auto serializedPayload = message->getSerializedPayload();
        auto payload = KitchenStatusDto::deserialize(serializedPayload);

        std::cout << "Message is: " + message->toString() << std::endl;
        std::cout << "Update time is: " << timePointToString(payload.updateTime) << std::endl;

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

    auto getStatusMessage = std::make_shared<IpcMessage>(IpcMessageType::GET_KITCHEN_STATUS_REQUEST, receptionIpcAddress,
                                                         IpcRoutingKeyHolder::GetKitchenStatus, ":");
    bus->publish(getStatusMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Set the stop flag to true and notify the condition variable
    bus->dispose();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
