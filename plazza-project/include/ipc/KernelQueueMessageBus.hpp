#ifndef KERNELQUEUEMESSAGEBUS_HPP
#define KERNELQUEUEMESSAGEBUS_HPP

#include "IMessageBus.hpp"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <atomic>

typedef std::function<void(std::shared_ptr<IpcMessage> &message)> IpcCallbackFunction;
typedef std::string IpcAddress;
typedef std::string IpcRoutingKey;
typedef std::unordered_map<IpcRoutingKey, IpcCallbackFunction> CallbackGroup;

class KernelQueueMessageBus : public IMessageBus {
protected:
    std::unordered_map<IpcAddress, IpcCallbackFunction> directCallbacks;
    std::unordered_map<IpcAddress, CallbackGroup> callbackGroups;

public:
    KernelQueueMessageBus() {
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        if (msgid == -1) {
            throw std::runtime_error("Failed to create message queue");
        }
    }

    ~KernelQueueMessageBus() {
        msgctl(msgid, IPC_RMID, nullptr);
    }

    void publish(const std::shared_ptr<IpcMessage> &message) override {

        std::lock_guard<std::mutex> lock(mtx); // Lock for send operations

        std::cout << "SENDING " + message->toString() << std::endl; // todo: remove tracing

        std::string serializedMessage = message->serialize();
        message_buf msg;
        msg.mtype = 1;
        snprintf(msg.mtext, sizeof(msg.mtext), "%s", serializedMessage.c_str());

        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            throw std::runtime_error("Failed to send message: " + message->toString());
        }

        cv.notify_all();
    }

    void runMessageHandling() override {
        while (!stopFlag) {
            message_buf msg;
            if (msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0) == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    throw std::runtime_error("Failed to receive message");
                }
            }

            std::string serializedMessage(msg.mtext);
            std::shared_ptr<IpcMessage> message = IpcMessage::deserialize(serializedMessage);

            // std::cout << "DESERIALIZED " + message->toString() << std::endl; // todo: remove tracing
            std::string ipcAddress = message->getIpcAddress();
            std::string routingKey = message->getRoutingKey();

            if (directCallbacks.find(ipcAddress) != directCallbacks.end()) {
                directCallbacks[ipcAddress](message);
            }
            else if (callbackGroups.find(ipcAddress) != callbackGroups.end()) {
                CallbackGroup targetCallbackGroup = callbackGroups[ipcAddress];
                if (targetCallbackGroup.find(routingKey) != targetCallbackGroup.end()) {
                    targetCallbackGroup[routingKey](message);
                }
                // todo: logging ?
            }
        }
    }

    void subscribe(const std::string &ipcAddress, const std::string &routingKey,
                   std::function<void(std::shared_ptr<IpcMessage> &message)> callback) override {
        callbackGroups[ipcAddress][routingKey] = callback;
    }

    void subscribe(const std::string &ipcAddress,
                   std::function<void(std::shared_ptr<IpcMessage> &message)> callback) override {
        directCallbacks[ipcAddress] = callback;
    }

    void dispose() override {
        stopFlag = true;
        cv.notify_all();
    }

private:
    struct message_buf {
        long mtype;
        char mtext[256];
    };

    key_t key;
    int msgid;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> stopFlag{false}; // Atomic flag to stop the message handling loop
};

#endif // KERNELQUEUEMESSAGEBUS_HPP
