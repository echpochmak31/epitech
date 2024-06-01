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

class KernelQueueMessageBus : public IMessageBus {
protected:
    std::unordered_map<std::string, std::function<void(std::shared_ptr<IpcMessage> &message)>> callbacks;

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

    void send(const std::shared_ptr<IpcMessage>& message) override {
        std::lock_guard<std::mutex> lock(mtx); // Lock for send operations

        std::cout << "SENDING " + message->toString() << std::endl;

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

            std::cout << "DESERIALIZED " + message->toString() << std::endl;

            std::string receiver = message->getReceiver();
            if (callbacks.find(receiver) != callbacks.end()) {
                callbacks[receiver](message);
            }
        }
    }

    void subscribe(const std::string &receiver, std::function<void(std::shared_ptr<IpcMessage> &message)> callback) override {
        callbacks[receiver] = callback;
    }

    void notifyStop() {
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
