// #include "Kitchen.hpp"
// #include <iostream>
//
// // Constructor
// Kitchen::Kitchen(std::string kitchenIpcAddress, std::string receptionIpcAddress,
//                  std::shared_ptr<IMessageBus> messageBus, int numCooks, int replenishTime, int idleTimeoutInSeconds)
//     : kitchenIpcAddress(kitchenIpcAddress),
//       receptionIpcAddress(receptionIpcAddress),
//       messageBus(messageBus),
//       _isAvailable(true),
//       idleTimeoutInSeconds(idleTimeoutInSeconds),
//       lastActiveTime(std::chrono::steady_clock::now()) {
//     for (int i = 0; i < numCooks; ++i) {
//         cooks.emplace_back();
//     }
//     monitoringThread = std::thread(&Kitchen::runMonitoringThread, this);
//     // monitoringThread.detach(); // fails
//
//     receivingThread = std::thread(&Kitchen::runReceivingThread, this); // Separate thread for receiving messages
//     // receivingThread.detach();
// }
//
// // Destructor
// Kitchen::~Kitchen() {
//     if (monitoringThread.joinable()) {
//         monitoringThread.join();
//     }
//     if (receivingThread.joinable()) {
//         receivingThread.join();
//     }
// }
//
// // Monitor the kitchen's idle status
// void Kitchen::runMonitoringThread() {
//     std::cout << "Kitchen monitoring thread started" << std::endl;
//     while (true) {
//         std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Check every 100 milliseconds
//         auto now = std::chrono::steady_clock::now();
//         auto idleDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastActiveTime).count();
//         if (idleDuration > idleTimeoutInSeconds) {
//             _isAvailable = false;
//             auto closeMessage = std::make_shared<IpcMessage>(IpcMessageType::CLOSE_KITCHEN, kitchenIpcAddress, receptionIpcAddress);
//             messageBus->send(closeMessage);
//             std::cout << "Kitchen sent CLOSE_KITCHEN message" << std::endl;
//             break;
//         }
//         std::cout << "Nothing special" << std::endl;
//     }
//     std::cout << "Kitchen monitoring thread exiting" << std::endl;
//     dispose();
// }
//
// // Separate thread for receiving messages
// void Kitchen::runReceivingThread() {
//     std::cout << "Kitchen receiving thread started" << std::endl;
//     while (true) {
//         auto receivedMessage = messageBus->receive(kitchenIpcAddress);
//         if (receivedMessage != nullptr) {
//             std::cout << "Kitchen received message: " << receivedMessage->toString() << std::endl;
//             onMessageReceived(receivedMessage);
//         }
//         if (!_isAvailable) {
//             break;
//         }
//     }
//     std::cout << "Kitchen receiving thread exiting" << std::endl;
// }
//
// // Check if kitchen is available
// bool Kitchen::isAvailable() {
//     for (const auto &cook: cooks) {
//         if (!cook.isBusy()) {
//             return true;
//         }
//     }
//     lastActiveTime = std::chrono::steady_clock::now();
//     _isAvailable = false;
//     return false;
// }
//
// // Count available cooks
// int Kitchen::countAvailableCooks() const {
//     int count = 0;
//     for (const auto &cook: cooks) {
//         if (!cook.isBusy()) {
//             ++count;
//         }
//     }
//     return count;
// }
//
// // Dispose kitchen resources
// void Kitchen::dispose() {
//     std::cout << "Kitchen is closing. Cleaning up resources..." << std::endl;
// }
//
// // Get total number of cooks
// int Kitchen::getCooksNumber() {
//     return cooks.size();
// }
//
// // Get replenish time (mock implementation)
// int Kitchen::getReplenishTime() {
//     return 0;
// }
//
// void Kitchen::runKitchen() {
//     for (int i = 0; i < cooks.size(); i++) {
//         if (!cooks[i].isBusy()) {
//             cooks[i].startCooking();
//         }
//     }
// }
//
// void Kitchen::onMessageReceived(const std::shared_ptr<IpcMessage>& ipcMessage) {
//     std::cout << "Processing received message: " << ipcMessage->toString() << std::endl;
//     if (ipcMessage->getMessageType().getValue() == IpcMessageType::GET_KITCHEN_STATUS) {
//         std::shared_ptr<IpcMessage> statusMessage;
//
//         if (_isAvailable) {
//             auto kitchenAvailableMessage = std::make_shared<IpcMessage>(
//                 IpcMessageType::KITCHEN_AVAILABLE, kitchenIpcAddress, receptionIpcAddress);
//             messageBus->send(kitchenAvailableMessage);
//         } else {
//             auto kitchenUnavailableMessage = std::make_shared<IpcMessage>(
//                 IpcMessageType::KITCHEN_UNAVAILABLE, kitchenIpcAddress, receptionIpcAddress);
//             messageBus->send(kitchenUnavailableMessage);
//         }
//     }
// }
