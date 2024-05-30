#include "IPC.hpp"

IPC::IPC() = default;

void IPC::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(message);
    queueCondVar.notify_one();
}

std::string IPC::receiveMessage() {
    std::unique_lock<std::mutex> lock(queueMutex);
    queueCondVar.wait(lock, [this] { return !messageQueue.empty(); });
    std::string message = messageQueue.front();
    messageQueue.pop();
    return message;
}
