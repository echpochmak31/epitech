#ifndef IMESSAGEBUS_HPP
#define IMESSAGEBUS_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "IpcMessage.hpp"

class IMessageBus {
public:
    virtual ~IMessageBus() = default;

    virtual void send(const std::shared_ptr<IpcMessage>& message) = 0;
    virtual std::shared_ptr<IpcMessage> receive(const std::string& receiver) = 0;
    virtual void registerReceiver(const std::string& receiver) = 0;
    virtual void unregisterReceiver(const std::string& receiver) = 0;

protected:
    std::unordered_map<std::string, std::queue<std::shared_ptr<IpcMessage>>> messageQueues;
    std::unordered_set<std::string> registeredReceivers;
    std::mutex mtx;
    std::condition_variable cv;
};

#endif // IMESSAGEBUS_HPP
