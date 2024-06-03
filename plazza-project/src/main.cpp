#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "Constants.hpp"
#include "Kitchen.hpp"
#include "Utils.hpp"
#include "ipc/KernelQueueMessageBus.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"
#include "ipc/Payloads.hpp"


int main() {
    float cookingTimeMultiplier = 1.0;

    auto mainLogger = std::make_shared<Logger>(std::cout);
    auto logger1 = std::make_shared<Logger>(std::cout);

    std::shared_ptr<IMessageBus> bus = std::make_shared<KernelQueueMessageBus>(mainLogger);

    std::string receptionIpcAddress = "reception";
    std::string kitchenIpcAddress = "kitchen1";

    auto orderedPizzaDto = OrderedPizzaDto();
    orderedPizzaDto.orderId = 1;
    orderedPizzaDto.type = PizzaType::Americana;
    orderedPizzaDto.orderId = PizzaSize::M;

    auto onKitchenStatusReceived = [&mainLogger](std::shared_ptr<IpcMessage> &message) {
        mainLogger->logWarning("Kitchen status: " + message->toString());
    };

    auto onKitchenClosed = [&mainLogger](std::shared_ptr<IpcMessage> &message) {
        mainLogger->logWarning("Kitchen closed " + message->getIpcAddress());
    };

    auto onOrderedPizzaReady = [&mainLogger, &orderedPizzaDto](std::shared_ptr<IpcMessage> &message) {
        auto serializedPayload = message->getSerializedPayload();
        auto payload = OrderedPizzaDto::deserialize(serializedPayload);
        if (orderedPizzaDto.orderId != payload.orderId) {
            mainLogger->logWarning("Ignoring...");
        }
        else {
            mainLogger->logWarning("Pizza ready!");
        }
    };

    auto kitchen1 = std::make_shared<Kitchen>(logger1, kitchenIpcAddress, bus, 2, cookingTimeMultiplier);

    auto getStatusMessage = std::make_shared<IpcMessage>(
        IpcMessageType::REQUEST,
        receptionIpcAddress,
        IpcRoutingKeyHolder::GetKitchenStatus,
        DUMMY_PAYLOAD);

    auto orderMessage = std::make_shared<IpcMessage>(
        IpcMessageType::SIGNAL,
        receptionIpcAddress,
        IpcRoutingKeyHolder::AcceptOrderedPizza,
        orderedPizzaDto.serialize());



    // subscribe on any message addressed to the kitchen
    bus->subscribe(receptionIpcAddress, std::bind(&Kitchen::handleMessage, kitchen1, std::placeholders::_1));

    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::CloseKitchen, onKitchenClosed);
    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, onKitchenStatusReceived);
    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::OrderedPizzaReady, onOrderedPizzaReady);

    // Start the message handling in a separate thread
    std::thread handlerThread(&IMessageBus::runMessageHandling, bus.get());

    bus->publish(orderMessage);
    bus->publish(orderMessage);
    bus->publish(orderMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    bus->publish(getStatusMessage);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    bus->publish(getStatusMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Set the stop flag to true and notify the condition variable
    bus->dispose();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
