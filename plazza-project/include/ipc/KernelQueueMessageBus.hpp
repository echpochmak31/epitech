#ifndef KERNELQUEUEMESSAGEBUS_HPP
#define KERNELQUEUEMESSAGEBUS_HPP

#include "IMessageBus.hpp"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <atomic>

typedef std::function<void(std::shared_ptr<IpcMessage> &message)> IpcCallbackFunction;
typedef std::string IpcAddress;
typedef std::string IpcRoutingKey;
typedef std::unordered_map<IpcRoutingKey, IpcCallbackFunction> CallbackGroup;

class KernelQueueMessageBus : public IMessageBus {
protected:
    std::shared_ptr<Logger> logger;
    std::shared_ptr<std::unordered_map<IpcAddress, IpcCallbackFunction>> directCallbacks;
    std::shared_ptr<std::unordered_map<IpcAddress, CallbackGroup>> callbackGroups;

    std::atomic<bool> isSleeping = false;
public:
    explicit KernelQueueMessageBus(std::shared_ptr<Logger> logger) : logger(logger) {
        key = ftok("progfile", 65);
        msgid = msgget(key, 0666 | IPC_CREAT);
        if (msgid == -1) {
            auto errorMessage = "Failed to create kernel message queue";
            logger->logError(errorMessage);
            throw std::runtime_error(errorMessage);
        }
        directCallbacks = std::make_shared<std::unordered_map<IpcAddress, IpcCallbackFunction>>();
        callbackGroups = std::make_shared<std::unordered_map<IpcAddress, CallbackGroup>>();
    }

    ~KernelQueueMessageBus() {
        msgctl(msgid, IPC_RMID, nullptr);
    }

    void publish(const std::shared_ptr<IpcMessage> &message) override {
        std::lock_guard<std::mutex> lock(mtx); // Lock for send operations

        logger->logDebug("Publishing message: " + message->serialize());

        std::string serializedMessage = message->serialize();
        message_buf msg;
        msg.mtype = 1;
        snprintf(msg.mtext, sizeof(msg.mtext), "%s", serializedMessage.c_str());

        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            auto errorMessage = "Failed to send message: " + message->serialize();
            logger->logError(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        cv.notify_all();
    }

    void runMessageHandling() override {
        while (!stopFlag) {

            // Lock and wait for condition variable if paused
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !isSleeping.load(); });
            }

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

            std::string receiverIpcAddress = message->getReceiver();
            std::string routingKey = message->getRoutingKey();

            {
                std::lock_guard<std::mutex> lock(callbackMtx); // Lock for access to directCallbacks and callbackGroups

                if (directCallbacks->find(receiverIpcAddress) != directCallbacks->end()) {
                    logger->logDebug("P2P callback has been found for message: " + serializedMessage);
                    (*directCallbacks)[receiverIpcAddress](message);
                    continue;
                }

                if (callbackGroups->find(receiverIpcAddress) != callbackGroups->end()) {
                    CallbackGroup targetCallbackGroup = (*callbackGroups)[receiverIpcAddress];
                    if (targetCallbackGroup.find(routingKey) != targetCallbackGroup.end()) {
                        logger->logDebug("Callback with routing key has been found for message: " + serializedMessage);
                        targetCallbackGroup[routingKey](message);
                        continue;
                    }
                }
            }

            logger->logError("No suitable callback was found for message: " + serializedMessage);
        }
    }

    void subscribe(const std::string &receiverIpcAddress, const std::string &routingKey,
                   std::function<void(std::shared_ptr<IpcMessage> &message)> callback) override {
        std::lock_guard<std::mutex> lock(callbackMtx); // Lock for modifying callbackGroups
        (*callbackGroups)[receiverIpcAddress][routingKey] = callback;
    }

    void subscribe(const std::string &receiverIpcAddress,
                   std::function<void(std::shared_ptr<IpcMessage> &message)> callback) override {
        std::lock_guard<std::mutex> lock(callbackMtx); // Lock for modifying directCallbacks
        (*directCallbacks)[receiverIpcAddress] = callback;
    }

    void dispose() override {
        stopFlag = true;
        cv.notify_all();
    }

    void pauseHandling() override {
        std::lock_guard<std::mutex> lock(mtx);
        isSleeping = true;
    }

    void resumeHandling() override {
        {
            std::lock_guard<std::mutex> lock(mtx);
            isSleeping = false;
        }
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
    std::mutex callbackMtx; // Mutex for protecting access to callback maps
    std::condition_variable cv;
    std::atomic<bool> stopFlag{false}; // Atomic flag to stop the message handling loop
};

#endif // KERNELQUEUEMESSAGEBUS_HPP
