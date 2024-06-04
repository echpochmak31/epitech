#include "Kitchen.hpp"
#include <iostream>
#include <mutex>

#include "Constants.hpp"
#include "RecipeBook.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"
#include "ipc/Payloads.hpp"

Kitchen::Kitchen(
    std::shared_ptr<Logger> logger,
    std::string ipcAddress,
    std::shared_ptr<IMessageBus> messageBus,
    KitchenParams params)

    : logger(logger),
      ipcAddress(ipcAddress),
      messageBus(messageBus),
      params(params),
      isClosing(false),
      lastActiveTime(std::chrono::steady_clock::now()),
      threadIsFree(params.cooksNumber, true),
      cookThreads(params.cooksNumber) {
    queuesHandlingThread = std::thread(&Kitchen::handleQueues, this);
}


Kitchen::~Kitchen() {
    isClosing = true;
    if (queuesHandlingThread.joinable()) {
        queuesHandlingThread.join();
    }
    for (auto &thread: cookThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void Kitchen::cookPizza(OrderedPizzaDto dto, int cookIndex) {
    const auto defaultCookingTime = RecipeBook::getCookingTimeInMilliseconds(dto.type, dto.size);
    const auto actualCookingTime = static_cast<float>(defaultCookingTime) * params.cookingTimeMultiplier;

    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(actualCookingTime)));

    cookedPizzas.enqueue(dto); {
        std::lock_guard<std::mutex> lock(mtx);
        threadIsFree[cookIndex] = true;
    }

    cv.notify_all();
}

void Kitchen::handleGetStatusRequest(std::shared_ptr<IpcMessage> &message) {
    auto payload = KitchenStatusDto();
    payload.available = isClosing;
    payload.availableCookNumber = getAvailableCookNumber();
    payload.totalCookNumber = params.cooksNumber;
    payload.updateTime = std::chrono::system_clock::now();
    payload.queuedPizzaNumber = static_cast<int>(orderedPizzas.size());

    auto response = std::make_shared<IpcMessage>(
        IpcMessageType::RESPONSE,
        ipcAddress,
        IpcRoutingKeyHolder::GetKitchenStatus,
        payload.serialize());

    return messageBus->publish(response);
}

void Kitchen::handleAcceptOrderedPizzaRequest(std::shared_ptr<IpcMessage> &message) {
    std::unique_lock<std::mutex> lock(mtx);

    if (orderedPizzas.size() >= params.cooksNumber * MAX_ORDERED_PIZZAS_MULTIPLIER) {
        logger->logError(
            "Amount of pizzas queued in Kitchen is more than it can handle. Kitchn IPC address: " + ipcAddress);
    }

    auto serializePayload = message->getSerializedPayload();
    auto orderedPizzaDto = OrderedPizzaDto::deserialize(serializePayload);
    orderedPizzas.enqueue(orderedPizzaDto);
}

void Kitchen::handleQueues() {
    while (!isClosing) {
        if (!cookedPizzas.empty()) {
            auto serializedCookedPizza = cookedPizzas.dequeue().serialize();

            auto message = std::make_shared<IpcMessage>(
                IpcMessageType::SIGNAL,
                ipcAddress,
                IpcRoutingKeyHolder::OrderedPizzaReady,
                serializedCookedPizza);

            messageBus->publish(message);
        }

        if (!orderedPizzas.empty()) {
            auto orderedPizzaDto = orderedPizzas.dequeue();
            assignOrderedPizza(orderedPizzaDto);
        }
    }
}


void Kitchen::assignOrderedPizza(OrderedPizzaDto orderedPizzaDto) {
    std::unique_lock<std::mutex> lock(mtx);

    for (int i = 0; i < threadIsFree.size(); ++i) {
        if (threadIsFree[i]) {
            lastActiveTime = std::chrono::steady_clock::now();
            threadIsFree[i] = false;
            cookThreads[i] = std::thread(&Kitchen::cookPizza, this, orderedPizzaDto, i); // Run cook thread
            cookThreads[i].detach();
            return;
        }
    }

    // If no cook is free, re-enqueue the pizza and wait for a cook to be free
    orderedPizzas.enqueue(orderedPizzaDto);
    cv.wait(lock, [this]() {
        return getAvailableCookNumber() > 0;
    });
}

void Kitchen::handleMessage(std::shared_ptr<IpcMessage> &message) {
    switch (message->getMessageType()) {
        case IpcMessageType::REQUEST:

            if (message->getRoutingKey() == IpcRoutingKeyHolder::GetKitchenStatus)
                handleGetStatusRequest(message);

            break;

        case IpcMessageType::SIGNAL:
            if (message->getRoutingKey() == IpcRoutingKeyHolder::AcceptOrderedPizza)
                handleAcceptOrderedPizzaRequest(message);

            break;

        default:
            std::ostringstream oss;
            oss << "Kitchen " << ipcAddress << " reports. Not supported IpcMessageType: "
                    << static_cast<int>(message->getMessageType());

            logger->logError(oss.str());

            break;
    }
}

int Kitchen::getAvailableCookNumber() const {
    int result = 0;
    for (const bool isFree: threadIsFree) {
        if (isFree) {
            result++;
        }
    }
    return result;
}
