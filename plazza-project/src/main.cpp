#include <iostream>
#include <memory>
#include <thread>

#include "ipc/CallbackSubscriber.hpp"
#include "ipc/KernelQueueMessageBus.hpp"


int main() {
    std::shared_ptr<IMessageBus> messageBus = std::make_shared<KernelQueueMessageBus>();
    // auto messageBus = std::make_shared<KernelQueueMessageBus>();

    std::string sender = "some sender";
    std::string routingKey = "routingKey1";

    auto callback1 = [](std::shared_ptr<IpcMessage> &message) {
        std::cout << "Callback invocation 1" << std::endl;
        std::cout << message->toString() << std::endl;
    };

    auto callback2 = [](std::shared_ptr<IpcMessage> &message) {
        std::cout << "Callback invocation 2" << std::endl;
        std::cout << message->toString() << std::endl;
    };

    std::cout << "test\n";
    auto subscriber = std::make_shared<CallbackSubscriber>(routingKey, callback1);
    messageBus->subscribe(subscriber);
    messageBus->subscribe(routingKey, callback2);

    // Start the message handling in a separate thread
    std::thread handlerThread(&IMessageBus::runMessageHandling, messageBus.get());
    std::cout << "ok\n";

    auto msg = std::make_shared<IpcMessage>(IpcMessageType::UNKNOWN, sender, routingKey);
    messageBus->publish(msg);

    std::cout << "done\n";

    // Set the stop flag to true and notify the condition variable
    messageBus->dispose();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
