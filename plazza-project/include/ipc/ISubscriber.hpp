#ifndef ISUBSCRIBER_HPP
#define ISUBSCRIBER_HPP

#include <memory>
#include <string>
#include "ipc/IpcMessage.hpp"

class ISubscriber {
public:
    virtual ~ISubscriber() = 0;
    virtual std::string getRoutingKey() = 0;
    virtual void handleMessage(std::shared_ptr<IpcMessage>& message) = 0;
};

inline ISubscriber::~ISubscriber() {}

#endif // ISUBSCRIBER_HPP
