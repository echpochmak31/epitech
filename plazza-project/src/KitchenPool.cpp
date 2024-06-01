// #include "KitchenPool.hpp"
// #include "Kitchen.hpp"
// #include <csignal>
// #include <iostream>
// #include <unistd.h>
// #include <sys/wait.h>
//
// KitchenPool::KitchenPool(int numCooks, int replenishTime, int initialPoolSize, int idleTimeLimitInSeconds)
// {
//     this->numCooks = numCooks;
//     this->replenishTime = replenishTime;
//     this->initailPoolSize = initialPoolSize;
//     this->idleTimeLimitInSeconds = idleTimeLimitInSeconds;
//
//     for (int i = 0; i < initialPoolSize; ++i) {
//         KitchenDetails details;
//         details.Pipe = std::make_shared<IPC>();
//         details.Pid = fork();
//         details.LastActiveTime = std::chrono::steady_clock::now();
//         details.isActive = false;
//
//         if (details.Pid == 0) { // Child process
//             close(details.Pipe->getReadFd());
//             Kitchen kitchen(numCooks, replenishTime);
//             kitchen.run(details.Pipe->getWriteFd());
//             close(details.Pipe->getWriteFd());
//             exit(0); // Exit after running the kitchen
//         } else { // Parent process
//             close(details.Pipe->getWriteFd());
//             kitchens.push_back(details);
//         }
//     }
//
//     // Start the monitoring thread
//     monitoringThread = std::thread(&KitchenPool::monitorKitchens, this);
// }
//
// KitchenPool::~KitchenPool() {
//     stopMonitoring = true;
//     if (monitoringThread.joinable()) {
//         monitoringThread.join();
//     }
//     for (auto& kitchen : kitchens) {
//         dispose(kitchen);
//     }
// }
//
// void KitchenPool::dispose(KitchenDetails &kitchenDetails) {
//     kill(kitchenDetails.Pid, SIGTERM);
//     waitpid(kitchenDetails.Pid, nullptr, 0);
//     kitchenDetails.Pipe.reset();
// }
//
// // KitchenDetails KitchenPool::getAvailableKitchen() {
// //     for (auto& kitchen : kitchens) {
// //         // Request busy cooks count
// //         kitchen.Pipe->operator<<("request_busy_cooks") ;
// //         std::string message;
// //         // kitchen.Pipe->receive(message);
// //         int busyCooks = std::stoi(message);
// //         if (busyCooks < numCooks) {
// //             kitchen.LastActiveTime = std::chrono::steady_clock::now();
// //             return kitchen;
// //         }
// //     }
// //
// //     // No available kitchen, create a new one
// //     KitchenDetails newKitchen;
// //     newKitchen.Pipe = std::make_shared<IPC>();
// //     newKitchen.Pid = fork();
// //     newKitchen.LastActiveTime = std::chrono::steady_clock::now();
// //     newKitchen.isActive = true;
// //
// //     if (newKitchen.Pid == 0) { // Child process
// //         close(newKitchen.Pipe->getReadFd());
// //         Kitchen k(numCooks, replenishTime, newKitchen.Pipe);
// //         k.run();
// //         close(newKitchen.Pipe->getWriteFd());
// //         exit(0); // Exit after running the kitchen
// //     } else { // Parent process
// //         close(newKitchen.Pipe->getWriteFd());
// //         kitchens.push_back(newKitchen);
// //     }
// //
// //     return newKitchen;
// // }
//
// // void KitchenPool::monitorKitchens() {
// //     while (!stopMonitoring) {
// //         auto now = std::chrono::steady_clock::now();
// //         for (auto& kitchen : kitchens) {
// //             // Request busy cooks count
// //             kitchen.Pipe->send("request_busy_cooks");
// //             std::string message;
// //             kitchen.Pipe->receive(message);
// //             int busyCooks = std::stoi(message);
// //             if (busyCooks == 0 && std::chrono::duration_cast<std::chrono::seconds>(now - kitchen.LastActiveTime).count() > idleTimeLimitInSeconds) {
// //                 dispose(kitchen);
// //             }
// //         }
// //         std::this_thread::sleep_for(std::chrono::seconds(1));
// //     }
// // }