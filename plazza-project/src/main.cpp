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


void test() {
    OrderedPizzaDto pizzaOrder;
    pizzaOrder.orderId = 1;
    pizzaOrder.type = PizzaType::Regina;
    pizzaOrder.size = PizzaSize::L;

    // Serialize the object
    std::string serializedData = pizzaOrder.serialize();
    std::cout << "Serialized Data: " << serializedData << std::endl;

    // Deserialize the object
    OrderedPizzaDto deserializedPizzaOrder = OrderedPizzaDto::deserialize(serializedData);
    std::cout << "Deserialized Data - Order ID: " << deserializedPizzaOrder.orderId
            << ", Type: " << static_cast<int>(deserializedPizzaOrder.type)
            << ", Size: " << static_cast<int>(deserializedPizzaOrder.size) << std::endl;
}

int main() {
    // test();
    float cookingTimeMultiplier = 1.0;

    std::shared_ptr<IMessageBus> bus = std::make_shared<KernelQueueMessageBus>();
    std::shared_ptr<Logger> mainLogger = std::make_shared<Logger>(std::cout);
    std::shared_ptr<Logger> logger1 = std::make_shared<Logger>(std::cout);

    std::string receptionIpcAddress = "reception";
    std::string kitchenIpcAddress = "kitchen1";

    auto onKitchenStatusReceived = [&mainLogger](std::shared_ptr<IpcMessage> &message) {
        if (message->getRoutingKey() != IpcRoutingKeyHolder::GetKitchenStatus) {
            mainLogger->logError("Invalid routing key");
            return;
        }

        auto serializedPayload = message->getSerializedPayload();
        auto payload = KitchenStatusDto::deserialize(serializedPayload);
        mainLogger->logInfo("Message is: " + message->toString());
        mainLogger->logInfo("Update time is: " + timePointToString(payload.updateTime));
    };

    auto onKitchenClosed = [&mainLogger](std::shared_ptr<IpcMessage> &message) {
        mainLogger->logInfo("Kitchen closed " + message->getIpcAddress());
    };

    auto kitchen1 = std::make_shared<Kitchen>(logger1, kitchenIpcAddress, bus, 2, cookingTimeMultiplier);


    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, onKitchenStatusReceived);

    // subscribe on any message addressed to the kitchen
    bus->subscribe(receptionIpcAddress, std::bind(&Kitchen::handleMessage, kitchen1, std::placeholders::_1));

    bus->subscribe(kitchenIpcAddress, IpcRoutingKeyHolder::CloseKitchen, onKitchenClosed);


    // Start the message handling in a separate thread
    std::thread handlerThread(&IMessageBus::runMessageHandling, bus.get());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto getStatusMessage = std::make_shared<IpcMessage>(
        IpcMessageType::RESPONSE,
        receptionIpcAddress,
        IpcRoutingKeyHolder::GetKitchenStatus,
        DUMMY_PAYLOAD);

    bus->publish(getStatusMessage);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Set the stop flag to true and notify the condition variable
    bus->dispose();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
