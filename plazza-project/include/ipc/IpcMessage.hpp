#ifndef BUSMESSAGE_HPP
#define BUSMESSAGE_HPP

#include <memory>
#include <string>
#include <sstream>
#include "IpcMessageType.hpp"

class IpcMessage {
protected:
    IpcMessageType type;
    std::string sender;
    std::string routingKey;

public:
    IpcMessage(IpcMessageType type, const std::string &sender, const std::string &routingKey)
        : sender(sender), routingKey(routingKey), type(type) {
    }

    ~IpcMessage() = default;

    std::string serialize() const {
        std::ostringstream oss;
        oss << sender << ":" << routingKey << ":" << type.toString();
        return oss.str();
    }

    static std::shared_ptr<IpcMessage> deserialize(const std::string &data) {
        std::istringstream iss(data);
        std::string sender, routingKey, typeStr;
        std::getline(iss, sender, ':');
        std::getline(iss, routingKey, ':');
        std::getline(iss, typeStr);
        IpcMessageType type = IpcMessageType::fromString(typeStr);

        return std::make_shared<IpcMessage>(type, sender, routingKey);
    }

    std::string getSender() const { return sender; }
    std::string getRoutingKey() const { return routingKey; }
    IpcMessageType getMessageType() const { return type; }

    std::string toString() const {
        return "type=" + getMessageType().toString() +
               "; sender=" + getSender() +
               "; routingKey=" + getRoutingKey() +
               ";";
    }
};

#endif //BUSMESSAGE_HPP
