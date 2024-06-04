#ifndef BUSMESSAGE_HPP
#define BUSMESSAGE_HPP

#include <memory>
#include <string>
#include <sstream>
#include "IpcMessageType.hpp"
#include "Constants.hpp"

class IpcMessage {
protected:
    std::string senderIpcAddress;
    std::string receiverIpcAddress;
    std::string routingKey;
    long dispatchTimeSinceEpoch;
    std::string serializedPayload;

public:
    IpcMessage(const std::string &senderIpcAddress, const std::string &receiverIpcAddress,
               const std::string &routingKey,
               const std::string &serializedPayload = DUMMY_PAYLOAD)
        : senderIpcAddress(senderIpcAddress), receiverIpcAddress(receiverIpcAddress), routingKey(routingKey),
          serializedPayload(serializedPayload) {
        dispatchTimeSinceEpoch = std::chrono::system_clock::now().time_since_epoch().count();
    }

    ~IpcMessage() = default;

    virtual std::string serialize() const {
        std::ostringstream oss;
        oss << senderIpcAddress << ":"
        << receiverIpcAddress << ":"
        << routingKey << ":"
        << dispatchTimeSinceEpoch << ":"
        << serializedPayload;
        return oss.str();
    }

    static std::shared_ptr<IpcMessage> deserialize(const std::string &data) {
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

    std::string getSender() const { return senderIpcAddress; }
    std::string getReceiver() const { return receiverIpcAddress; }
    std::string getRoutingKey() const { return routingKey; }
    long getDispatchTimeSinceEpoch() const { return dispatchTimeSinceEpoch; }
    std::string getSerializedPayload() const { return serializedPayload; }

};

#endif //BUSMESSAGE_HPP
