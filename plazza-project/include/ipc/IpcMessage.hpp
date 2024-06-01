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

public:
    IpcMessage(IpcMessageType type, const std::string &ipcAddress, const std::string &routingKey)
        : ipcAddress(ipcAddress), routingKey(routingKey), type(type) {
    }

    ~IpcMessage() = default;

    std::string serialize() const {
        std::ostringstream oss;
        oss << ipcAddress << ":" << routingKey << ":" << type.toString();
        return oss.str();
    }

    static std::shared_ptr<IpcMessage> deserialize(const std::string &data) {
        std::istringstream iss(data);
        std::string ipcAddress, routingKey, typeStr;
        std::getline(iss, ipcAddress, ':');
        std::getline(iss, routingKey, ':');
        std::getline(iss, typeStr);
        IpcMessageType type = IpcMessageType::fromString(typeStr);

        return std::make_shared<IpcMessage>(type, ipcAddress, routingKey);
    }

    std::string getIpcAddress() const { return ipcAddress; }
    std::string getRoutingKey() const { return routingKey; }
    IpcMessageType getMessageType() const { return type; }

    std::string toString() const {
        return "type=" + getMessageType().toString() +
               "; ipcAddress=" + getIpcAddress() +
               "; routingKey=" + getRoutingKey() +
               ";";
    }
};

#endif //BUSMESSAGE_HPP
