#ifndef BUSMESSAGE_HPP
#define BUSMESSAGE_HPP

#include <memory>
#include <string>
#include <sstream>
#include "IpcMessageType.hpp"

class IpcMessage {
protected:
    IpcMessageType type;
    std::string ipcAddress;
    std::string routingKey;
    std::string serializedPayload;

public:
    IpcMessage(IpcMessageType type, const std::string &ipcAddress, const std::string &routingKey,
               const std::string &serializedPayload)
        : type(type), ipcAddress(ipcAddress), routingKey(routingKey), serializedPayload(serializedPayload) {
    }

    ~IpcMessage() = default;

    virtual std::string serialize() const {
        std::ostringstream oss;
        oss << ipcAddress << ":" << routingKey << ":" << static_cast<int>(type) << ":" << serializedPayload;
        return oss.str();
    }

    static std::shared_ptr<IpcMessage> deserialize(const std::string &data) {
        std::istringstream iss(data);
        std::string ipcAddress, routingKey, typeStr, serializedPayload;
        std::getline(iss, ipcAddress, ':');
        std::getline(iss, routingKey, ':');
        std::getline(iss, typeStr, ':');
        std::getline(iss, serializedPayload);
        auto typeValue = static_cast<IpcMessageType>(std::stoi(typeStr));
        auto type = IpcMessageType(typeValue);

        return std::make_shared<IpcMessage>(type, ipcAddress, routingKey, serializedPayload);
    }

    std::string getIpcAddress() const { return ipcAddress; }
    std::string getRoutingKey() const { return routingKey; }
    IpcMessageType getMessageType() const { return type; }
    std::string getSerializedPayload() const { return serializedPayload; }

    virtual std::string toString() const {
        std::ostringstream oss;
        oss
        << "type=" << static_cast<int>(type) << ";"
        << "ipcAddress=" << getIpcAddress() << ";"
        << "routingKey=" << getRoutingKey() << ";"
        << "serializedPayload=" << getSerializedPayload() << ";";
        return oss.str();
    }
};

#endif //BUSMESSAGE_HPP
