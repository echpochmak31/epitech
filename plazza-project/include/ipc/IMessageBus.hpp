#ifndef IMESSAGEBUS_HPP
#define IMESSAGEBUS_HPP

#include <functional>
#include <memory>
#include <string>
#include "IpcMessage.hpp"
#include "ISubscriber.hpp"

class IMessageBus {
public:
    virtual ~IMessageBus() = default;

    virtual void publish(const std::shared_ptr<IpcMessage> &message) = 0;

    virtual void subscribe(std::shared_ptr<ISubscriber> subscriber) = 0;

    virtual void subscribe(const std::string &routingKey,
                           std::function<void(std::shared_ptr<IpcMessage> &message)> callback) = 0;

    virtual void runMessageHandling() = 0;

    virtual void dispose() = 0;
};

#endif // IMESSAGEBUS_HPP
