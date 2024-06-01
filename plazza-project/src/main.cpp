#include <algorithm>
#include <iostream>
#include <memory>
#include "Kitchen.hpp"
#include "ipc/KernelQueueMessageBus.hpp"
#include <unistd.h>

int main() {
    auto messageBus = std::make_shared<KernelQueueMessageBus>();
    auto kitchenAddr = "kitchen1";
    auto receptionAddr = "reception";

    messageBus->registerReceiver(kitchenAddr);
    messageBus->registerReceiver(receptionAddr);

    Kitchen kitchen(kitchenAddr, receptionAddr, messageBus, 3, 10, 2);

    // Fork process for kitchen
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        // std::this_thread::sleep_for(std::chrono::seconds(10)); // Simulate some delay to trigger idle timeout
        exit(0);
    } else if (pid > 0) {
        // Parent process
        std::cout << "Kitchen process started with PID: " << pid << std::endl;
        while (true) {
            // auto pollMsgRequest = std::make_shared<IpcMessage>(IpcMessageType::GET_KITCHEN_STATUS, receptionAddr, kitchenAddr);
            // messageBus->send(pollMsgRequest);

            // std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate some delay
            auto pollMsgResponse = messageBus->receive(receptionAddr);
            if (pollMsgResponse != nullptr) {
                std::cout << pollMsgResponse->getMessageType().toString() << std::endl;
                break;
            }

            // std::cout << pollMsgResponse->getMessageType().toString() << "\n";
            // std::cout << "Kitchen availability: " << kitchen. << std::endl;
            // std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate some delay
            // break;
        }
    } else {
        // Fork failed
        std::cerr << "Failed to fork process for kitchen." << std::endl;
        return 1;
    }

    return 0;
}
