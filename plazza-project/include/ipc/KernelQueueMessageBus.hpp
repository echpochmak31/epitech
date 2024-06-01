#ifndef KERNELQUEUEMESSAGEBUS_HPP
#define KERNELQUEUEMESSAGEBUS_HPP

#include "IMessageBus.hpp"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdexcept>
#include <iostream>
#include <cstring>

class KernelQueueMessageBus : public IMessageBus {
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

    void send(const std::shared_ptr<IpcMessage> &message) override {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << message->toString() << std::endl; // todo: remove tracing

        if (registeredReceivers.find(message->getReceiver()) == registeredReceivers.end()) {
            throw std::runtime_error(
                "Receiver not registered for message: " + message->toString() );
        }
        std::string serializedMessage = message->serialize();
        message_buf msg;
        msg.mtype = 1;
        snprintf(msg.mtext, sizeof(msg.mtext), "%s", serializedMessage.c_str());

        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            throw std::runtime_error(
                "Failed to send message: " + message->toString());
        }

        messageQueues[message->getReceiver()].push(message);
        cv.notify_all();
    }

    std::shared_ptr<IpcMessage> receive(const std::string &receiver) override {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return !messageQueues[receiver].empty(); });

        if (messageQueues[receiver].empty()) {
            return nullptr;
        }

        auto message = messageQueues[receiver].front();
        messageQueues[receiver].pop();
        return message;
    }

    void registerReceiver(const std::string &receiver) override {
        std::lock_guard<std::mutex> lock(mtx);
        registeredReceivers.insert(receiver);
    }

    void unregisterReceiver(const std::string &receiver) override {
        std::lock_guard<std::mutex> lock(mtx);
        registeredReceivers.erase(receiver);
        messageQueues.erase(receiver);
    }

private:
    struct message_buf {
        long mtype;
        char mtext[256];
    };

    key_t key;
    int msgid;
};

#endif // KERNELQUEUEMESSAGEBUS_HPP
