#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include "ipc/KernelQueueMessageBus.hpp"

int main() {
    auto messageBus = std::make_shared<KernelQueueMessageBus>();
    std::string sender = "some sender";
    std::string receiver = "topic1";

    // Start the message handling in a separate thread
    std::thread handlerThread(&KernelQueueMessageBus::runMessageHandling, messageBus);
    std::cout << "test\n";

    messageBus->subscribe(receiver, [](std::shared_ptr<IpcMessage> &message) {
        std::cout << message->toString() << std::endl;
    });

    std::cout << "ok\n";

    auto msg = std::make_shared<IpcMessage>(IpcMessageType::UNKNOWN, sender, receiver);
    messageBus->send(msg);

    std::cout << "done\n";

    return 0;
}
