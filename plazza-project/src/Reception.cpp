#include "Reception.hpp"
#include "KitchenDetails.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"
#include <cmath>
#include <set>

// todo: refactor constructor
// Reception::Reception(ThreadSafeQueue<Order> &queuedOrders, KitchenParams kitchenParams,
//                      std::shared_ptr<IMessageBus> messageBus, std::string ipcAddress) {
//     receptionEnabled = true;
// }

Reception::~Reception() {
    receptionEnabled = false;
}

void Reception::decomposeOrder(Order &order) {
    for (int i = 0; i < order.totalPizzaNumber; i++) {
        auto dto = OrderedPizzaDto();
        dto.orderId = order.id;
        dto.type = order.types[i];
        dto.size = order.sizes[i];

        queuedPizzas->enqueue(dto);
    }
}

std::vector<std::pair<IpcAddress, KitchenStatusDto> > Reception::pollKitchens() {
    std::unique_lock<std::mutex> lock(mtx);

    std::vector<std::pair<IpcAddress, KitchenStatusDto> > responses;

    auto onStatusReceived = [this, &responses](std::shared_ptr<IpcMessage> &message) {
        if (message->getMessageType() != IpcMessageType::RESPONSE) {
            auto errorMessage = "Invalid message type received. RESPONSE was expected, but actual is: "
                                + static_cast<int>(message->getMessageType());
            logger->logError(errorMessage);
            return;
        }
        auto data = message->getSerializedPayload();
        auto payload = KitchenStatusDto::deserialize(data);
        responses.push_back(std::make_pair(message->getIpcAddress(), payload));
    };

    size_t fixedKitchenNumber = kitchenPool.size();
    logger->logDebug("Start polling. Response is expected for this number of kitchens: " + fixedKitchenNumber);

    for (const auto &[Pid, IpcAddress]: kitchenPool) {
        messageBus->subscribe(IpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, onStatusReceived);

        auto getStatusRequest = std::make_shared<IpcMessage>(
            IpcMessageType::REQUEST,
            IpcAddress,
            IpcRoutingKeyHolder::GetKitchenStatus);

        messageBus->publish(getStatusRequest);
    }

    cv.wait_for(lock, std::chrono::seconds(POLLING_TIMEOUT_IN_SECONDS), [&responses, fixedKitchenNumber] {
        return responses.size() == fixedKitchenNumber;
    });

    if (responses.size() == fixedKitchenNumber) {
        return responses;
    }

    // Handle faulty requests

    std::ostringstream wargingMessage;
    wargingMessage << "Some kitchens failed to respond. " << responses.size() << " out of "
            << static_cast<int>(fixedKitchenNumber) << " expected responses has been received.";
    logger->logWarning(wargingMessage.str());

    // todo: track faulty kitchens and close them after a while
}

void Reception::assignPizza(IpcAddress kitchenIpcAddress, OrderedPizzaDto orderedPizzaDto) {
    auto message = std::make_shared<IpcMessage>(
        IpcMessageType::SIGNAL,
        kitchenIpcAddress,
        IpcRoutingKeyHolder::AcceptOrderedPizza,
        orderedPizzaDto.serialize());

    messageBus->publish(message);
}

void Reception::distributeOrderedPizzas(std::vector<std::pair<IpcAddress, int> > targetKitchens) {
    int commonCapacity = 0;
    std::unordered_map<IpcAddress, int> currentCapacities;

    for (const auto &[kitchenIpcAddress, kitchenCapacity]: targetKitchens) {
        commonCapacity += kitchenCapacity;
        currentCapacities[kitchenIpcAddress] = kitchenCapacity;
    }


    // While there are any queued pizzas and any target kitchen can accept a pizza
    while (!queuedPizzas->empty() && commonCapacity > 0) {
        // Kind of Round Robin for distributing pizzas evenly
        for (auto [kitchenIpcAddress, kitchenCapacity]: targetKitchens) {
            if (currentCapacities[kitchenIpcAddress] == 0)
                continue;

            auto orderedPizzaDto = queuedPizzas->dequeue();
            commonCapacity--;
            currentCapacities[kitchenIpcAddress]--;
            assignPizza(kitchenIpcAddress, orderedPizzaDto);
        }
    }
}

void Reception::bindMessageHandlers() {
    // todo
}

KitchenDetails Reception::createNewKitchen() {
    // todo
}

void Reception::closeKitchen(IpcAddress kitchenIpcAddress) {
    // todo
}

void Reception::runOrderedPizzasAssigning() {
    std::vector<std::pair<IpcAddress, int> > targetKitchens; // address - number of pizzas ready to accept

    while (receptionEnabled) {
        pizzasLeftToCookByOrder.clear();
        int totalPizzaCountReadyToAccept = 0;
        int fixedQueuedPizzasSize = queuedPizzas->size();

        auto statuses = pollKitchens();
        std::set<IpcAddress> idleKitchens;

        for (const auto &[ipcAddress, status]: statuses) {
            auto now = std::chrono::system_clock::now();
            auto lastActiveTime = std::chrono::system_clock::from_time_t(
                status.lastActiveTime.time_since_epoch().count());
            auto idleDuration = std::chrono::duration_cast<std::chrono::seconds>(lastActiveTime - now).count();

            if (idleDuration > KITCHEN_IDLE_TIME_LIMIT_IN_SECONDS && status.queuedPizzaNumber == 0) {
                idleKitchens.insert(ipcAddress);
            }

            // if the kitchen is not idle
            if (idleKitchens.find(ipcAddress) == idleKitchens.end()) {
                int count = status.totalCookNumber * MAX_ORDERED_PIZZAS_MULTIPLIER - status.queuedPizzaNumber;
                if (count > 0) {
                    totalPizzaCountReadyToAccept += count;
                    targetKitchens.push_back(std::make_pair(ipcAddress, count));
                }
            }
        }

        // Close idle kitchens
        for (auto idleKitchenIpcAddress: idleKitchens) {
            closeKitchen(idleKitchenIpcAddress);
        }

        int deficiency = fixedQueuedPizzasSize - totalPizzaCountReadyToAccept;

        // Create new kitchens and add it to targets
        if (deficiency > 0) {
            int newKitchensCount = std::ceil(static_cast<double>(deficiency) / 3);
            for (int i = 0; i < newKitchensCount; i++) {
                auto kitchenDetails = createNewKitchen();
                targetKitchens.push_back(std::make_pair(kitchenDetails.IpcAddress,
                                                        kitchenParams.cooksNumber * MAX_ORDERED_PIZZAS_MULTIPLIER));
            }
        }

        distributeOrderedPizzas(targetKitchens);
    }
}

void Reception::runOrderCheck() {
    while (receptionEnabled) {
        for (auto [orderId, count]: pizzasLeftToCookByOrder) {
            if (count == 0) {
                std::lock_guard<std::mutex> lock(ordersByIdMtx);
                auto executedOrder = ordersById[orderId];
                executedOrders->enqueue(executedOrder);
                pizzasLeftToCookByOrder.erase(orderId);
                ordersById.erase(orderId);
            }
        }
    }
}

void Reception::runOrderHandling() {
    while (receptionEnabled) {
        if (queuedOrders->empty())
            continue;

        auto order = queuedOrders->dequeue();

        {
            std::lock_guard<std::mutex> lockOrdersById(ordersByIdMtx);
            ordersById[order.id] = order;
        }

        {
            std::lock_guard<std::mutex> lockPizzasLeftToCookByOrder(pizzasLeftToCookByOrderMtx);
            pizzasLeftToCookByOrder[order.id] = order.totalPizzaNumber;
        }

        decomposeOrder(order);
    }
}
