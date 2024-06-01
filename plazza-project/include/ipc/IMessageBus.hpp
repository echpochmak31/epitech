#ifndef IMESSAGEBUS_HPP
#define IMESSAGEBUS_HPP

#include <memory>
#include <string>
#include <condition_variable>
#include "IpcMessage.hpp"

class IMessageBus {
public:
    virtual ~IMessageBus() = default;

    virtual void send(const std::shared_ptr<IpcMessage> &message) = 0;

    virtual void subscribe(std::string &receiver,
                           std::function<void(std::shared_ptr<IpcMessage> &message)> callback) = 0;

    virtual void runMessageHandling() = 0;
};

#endif // IMESSAGEBUS_HPP
