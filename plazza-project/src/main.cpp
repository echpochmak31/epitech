#include <iostream>
#include <memory>
#include <thread>
#include "ipc/KernelQueueMessageBus.hpp"

int main() {
    // std::shared_ptr<IMessageBus> messageBus = std::make_shared<KernelQueueMessageBus>();
    auto messageBus = std::make_shared<KernelQueueMessageBus>();

    std::string sender = "some sender";
    std::string receiver = "topic1";

    // Start the message handling in a separate thread
    std::thread handlerThread(&KernelQueueMessageBus::runMessageHandling, messageBus.get());
    std::cout << "test\n";

    messageBus->subscribe(receiver, [](std::shared_ptr<IpcMessage> &message) {
        std::cout << "Callback invocation" << std::endl;
        std::cout << message->toString() << std::endl;
    });

    std::cout << "ok\n";

    auto msg = std::make_shared<IpcMessage>(IpcMessageType::UNKNOWN, sender, receiver);
    messageBus->send(msg);

    std::cout << "done\n";

    // Set the stop flag to true and notify the condition variable
    messageBus->notifyStop();

    // Join the handler thread
    handlerThread.join();

    return 0;
}
