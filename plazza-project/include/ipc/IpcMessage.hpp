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
    std::string receiver;

public:
    IpcMessage(IpcMessageType type, const std::string &sender, const std::string &receiver)
        : sender(sender), receiver(receiver), type(type) {
    }

    virtual ~IpcMessage() = default;

    virtual std::string serialize() const {
        std::ostringstream oss;
        oss << sender << ":" << receiver << ":" << type.toString();
        return oss.str();
    }

    static std::shared_ptr<IpcMessage> deserialize(const std::string &data) {
        std::istringstream iss(data);
        std::string sender, receiver, typeStr;
        std::getline(iss, sender, ':');
        std::getline(iss, receiver, ':');
        std::getline(iss, typeStr);
        IpcMessageType type = IpcMessageType::UNKNOWN; // Modify this to properly parse the typeStr into IpcMessageType

        return std::make_shared<IpcMessage>(type, sender, receiver);
    }

    std::string getSender() const { return sender; }
    std::string getReceiver() const { return receiver; }
    IpcMessageType getMessageType() const { return type; }

    std::string toString() const {
        return "type=" + getMessageType().toString() +
               "; sender=" + getSender() +
               "; receiver=" + getReceiver() +
               ";";
    }
};


#endif //BUSMESSAGE_HPP
