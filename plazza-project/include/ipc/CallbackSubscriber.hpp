#ifndef CALLBACKSUBSCRIBER_HPP
#define CALLBACKSUBSCRIBER_HPP

#include <functional>
#include <memory>
#include <string>
#include "ipc/IpcMessage.hpp"
#include "ipc/ISubscriber.hpp"

class CallbackSubscriber final : public ISubscriber {
protected:
    std::string routingKey;
    std::function<void(std::shared_ptr<IpcMessage> &message)> callback;

public:
    ~CallbackSubscriber() override;

    CallbackSubscriber(std::string routingKey,
                       const std::function<void(std::shared_ptr<IpcMessage> &message)> &callback);

    std::string getRoutingKey() override;

    void handleMessage(std::shared_ptr<IpcMessage> &message) override;
};

#endif // CALLBACKSUBSCRIBER_HPP
