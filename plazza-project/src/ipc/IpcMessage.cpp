#include "ipc/IpcMessage.hpp"
#include <chrono>

IpcMessage::IpcMessage(const std::string &senderIpcAddress, const std::string &receiverIpcAddress,
                       const std::string &routingKey, const std::string &serializedPayload)
    : senderIpcAddress(senderIpcAddress), receiverIpcAddress(receiverIpcAddress), routingKey(routingKey),
      serializedPayload(serializedPayload) {
    dispatchTimeSinceEpoch = std::chrono::system_clock::now().time_since_epoch().count();
}

std::string IpcMessage::serialize() const {
    std::ostringstream oss;
    oss << senderIpcAddress << ":"
        << receiverIpcAddress << ":"
        << routingKey << ":"
        << dispatchTimeSinceEpoch << ":"
        << serializedPayload;
    return oss.str();
}

std::shared_ptr<IpcMessage> IpcMessage::deserialize(const std::string &data) {
    std::istringstream iss(data);
    std::string sender, receiver, routingKey, dispatchTimeStr, serializedPayload;

    std::getline(iss, sender, ':');
    std::getline(iss, receiver, ':');
    std::getline(iss, routingKey, ':');
    std::getline(iss, dispatchTimeStr, ':');
    std::getline(iss, serializedPayload);

    auto result = std::make_shared<IpcMessage>(sender, receiver, routingKey, serializedPayload);
    result->dispatchTimeSinceEpoch = std::stol(dispatchTimeStr);

    return result;
}

std::string IpcMessage::getSender() const {
    return senderIpcAddress;
}

std::string IpcMessage::getReceiver() const {
    return receiverIpcAddress;
}

std::string IpcMessage::getRoutingKey() const {
    return routingKey;
}

long IpcMessage::getDispatchTimeSinceEpoch() const {
    return dispatchTimeSinceEpoch;
}

std::string IpcMessage::getSerializedPayload() const {
    return serializedPayload;
}
