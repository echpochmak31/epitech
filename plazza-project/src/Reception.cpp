#include "Reception.hpp"
#include "KitchenDetails.hpp"
#include "ipc/IpcRoutingKeyHolder.hpp"
#include <cmath>
#include <set>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "Kitchen.hpp"
#include "Utils.hpp"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

size_t Reception::totalKitchensCreated = 0;


Reception::Reception(std::shared_ptr<Logger> logger,
                     std::shared_ptr<ThreadSafeQueue<Order> > queuedOrders,
                     KitchenParams kitchenParams,
                     std::shared_ptr<IMessageBus> messageBus,
                     std::shared_ptr<std::unordered_map<OrderId, std::string>> unparsedOrders) : receptionEnabled(true),
    logger(logger),
    kitchenParams(kitchenParams),
    receptionIpcAddress(RECEPTION_DEFAULT_IPC_ADDRESS),
    messageBus(messageBus),
    queuedOrders(queuedOrders),
    unparsedOrders(unparsedOrders) {

    queuedPizzas = std::make_shared<ThreadSafeQueue<OrderedPizzaDto>>();
    logger->logDebug("Reception has been instantiated");
}

Reception::~Reception() {
    receptionEnabled = false;
}

void Reception::decomposeOrder(const Order &order) const {
    logger->logDebug("Decomposing order: " + order.id);

    for (int i = 0; i < order.totalPizzaNumber; i++) {
        auto dto = OrderedPizzaDto();
        dto.orderId = order.id;
        dto.type = order.types[i];
        dto.size = order.sizes[i];

        queuedPizzas->enqueue(dto);
    }
}

std::vector<std::pair<IpcAddress, KitchenStatusDto> > Reception::pollKitchens() {
    std::vector<std::pair<IpcAddress, KitchenStatusDto>> responses;

    auto onStatusReceived = [this, &responses](std::shared_ptr<IpcMessage> &message) {
        if (message->getRoutingKey() != IpcRoutingKeyHolder::GetKitchenStatus) {
            std::ostringstream errorMessage;
            errorMessage << "Unexpected message received. Expected routing key is: "
            << IpcRoutingKeyHolder::GetKitchenStatus << " Actual is: " << message->getRoutingKey();
            logger->logError(errorMessage.str());
            return;
        }
        auto data = message->getSerializedPayload();
        auto payload = KitchenStatusDto::deserialize(data);
        responses.push_back(std::make_pair(message->getSender(), payload));
    };

    size_t fixedKitchenNumber = kitchenPool.size();

    messageBus->subscribe(receptionIpcAddress, IpcRoutingKeyHolder::GetKitchenStatus, onStatusReceived);

    for (const auto &[Pid, IpcAddress]: kitchenPool) {
        auto getStatusRequest = std::make_shared<IpcMessage>(
            receptionIpcAddress,
            IpcAddress,
            IpcRoutingKeyHolder::GetKitchenStatus);

        messageBus->publish(getStatusRequest);
    }

    auto timeout = std::chrono::seconds(POLLING_TIMEOUT_IN_SECONDS);

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, timeout, [&responses, fixedKitchenNumber] {
            return responses.size() == fixedKitchenNumber;
        });
    }

    // Handle faulty requests
    if (responses.size() != fixedKitchenNumber) {
        std::ostringstream wargingMessage;
        wargingMessage << "Some kitchens failed to respond. " << responses.size() << " out of "
                << static_cast<int>(fixedKitchenNumber) << " expected responses has been received.";
        logger->logWarning(wargingMessage.str());

        // todo: track faulty kitchens and close them after a while
    }

    return responses;
}

void Reception::assignPizza(IpcAddress kitchenIpcAddress, OrderedPizzaDto orderedPizzaDto) {
    auto message = std::make_shared<IpcMessage>(
        receptionIpcAddress,
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

KitchenDetails Reception::createNewKitchen() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        auto errorMessage = "Failed to create pipe for synchronization";
        logger->logError(errorMessage);
        throw std::runtime_error(errorMessage);
    }

    int pid = fork();
    if (pid < 0) {
        close(pipefd[0]);
        close(pipefd[1]);
        auto errorMessage = "Failed to fork new kitchen process";
        logger->logError(errorMessage);
        throw std::runtime_error(errorMessage);
    }

    totalKitchensCreated++;
    auto kitchenIpcAddress = makeKitchenIpcAddress(totalKitchensCreated);

    // Child process
    if (pid == 0) {
        close(pipefd[0]); // Close read end of the pipe in the child

        auto kitchen = std::make_shared<Kitchen>(logger, kitchenIpcAddress, messageBus, kitchenParams);

        // Notify the parent that the kitchen is ready
        write(pipefd[1], "1", 1);
        close(pipefd[1]); // Close write end of the pipe in the child

        exit(0);
    }

    close(pipefd[1]); // Close write end of the pipe in the parent

    // Parent process
    KitchenDetails kitchenDetails = {pid, kitchenIpcAddress};
    kitchenPool.push_back(kitchenDetails);

    // Wait for the child to signal that it is ready
    char buffer;
    read(pipefd[0], &buffer, 1);
    close(pipefd[0]); // Close read end of the pipe in the parent

    auto onCloseKitchen = [this, kitchenIpcAddress](std::shared_ptr<IpcMessage>& message) {
        logger->logInfo("Closing kitchen with IPC address: " + kitchenIpcAddress);
        closeKitchen(kitchenIpcAddress);
    };

    auto onOrderedPizzaReady = [this, kitchenIpcAddress](std::shared_ptr<IpcMessage>& message) {
        auto serializedPayload = message->getSerializedPayload();
        auto payload = OrderedPizzaDto::deserialize(serializedPayload);
        pizzasLeftToCookByOrder[payload.orderId]--;

        if (pizzasLeftToCookByOrder[payload.orderId] == 0) {
            std::ostringstream orderReadyMessage;
            orderReadyMessage << "Order with ID: " << payload.orderId << " is ready!";
            logger->logInfo(orderReadyMessage.str());
            handleExecutedOrder(payload.orderId);
        }
    };

    // Subscribe reception to message handling
    messageBus->subscribe(receptionIpcAddress, IpcRoutingKeyHolder::CloseKitchen, onCloseKitchen);
    messageBus->subscribe(receptionIpcAddress, IpcRoutingKeyHolder::OrderedPizzaReady, onOrderedPizzaReady);

    // Subscribe kitchen to message handling
    messageBus->subscribe(kitchenIpcAddress, std::bind(&Kitchen::handleMessage, std::make_shared<Kitchen>(logger, kitchenIpcAddress, messageBus, kitchenParams), std::placeholders::_1));

    logger->logInfo("New kitchen created with PID: " + std::to_string(pid));

    return kitchenDetails;
}

void Reception::closeKitchen(IpcAddress kitchenIpcAddress) {
    auto it = std::find_if(kitchenPool.begin(), kitchenPool.end(),
                           [&kitchenIpcAddress](const KitchenDetails &kd) {
                               return kd.IpcAddress == kitchenIpcAddress;
                           });

    if (it != kitchenPool.end()) {
        pid_t pid = it->Pid;

        // Kill the child process
        if (kill(pid, SIGTERM) == -1) {
            auto errorMessage = "Failed to terminate kitchen process with PID: " + std::to_string(pid);
            logger->logError(errorMessage);
            throw std::runtime_error(errorMessage);
        }

        // Remove the kitchen details from the pool
        kitchenPool.erase(it);

        // todo: Unsubscribe from the message bus
        // messageBus->unsubscribe(kitchenIpcAddress);

        logger->logDebug("Kitchen closed with PID: " + std::to_string(pid) + ", IPC Address: " + kitchenIpcAddress);
    } else {
        auto errorMessage = "Kitchen with IPC Address: " + kitchenIpcAddress + " not found";
        logger->logError(errorMessage);
        throw std::runtime_error(errorMessage);
    }
}

void Reception::run() {
    while (receptionEnabled) {
        handleOrders();
        assignPizzas();
    }
}

void Reception::handleOrders() {
    while (!queuedOrders->empty()) {
        auto order = queuedOrders->dequeue();
        ordersById[order.id] = order;
        pizzasLeftToCookByOrder[order.id] = order.totalPizzaNumber;
        decomposeOrder(order);
    }
}

void Reception::assignPizzas() {
    std::vector<std::pair<IpcAddress, int> > targetKitchens; // address - number of pizzas ready to accept

    int totalPizzaCountReadyToAccept = 0;
    int fixedQueuedPizzasSize = static_cast<int>(queuedPizzas->size());

    auto statuses = pollKitchens();

    for (const auto &[ipcAddress, status]: statuses) {
        int count = status.totalCookNumber * MAX_ORDERED_PIZZAS_MULTIPLIER - status.queuedPizzaNumber;
        if (count > 0) {
            totalPizzaCountReadyToAccept += count;
            targetKitchens.push_back(std::make_pair(ipcAddress, count));
        }
    }

    int deficiency = fixedQueuedPizzasSize - totalPizzaCountReadyToAccept;

    // Create new kitchens and add it to targets
    if (deficiency > 0) {
        int newKitchensCount = std::ceil(static_cast<double>(deficiency) / (kitchenParams.cooksNumber * MAX_ORDERED_PIZZAS_MULTIPLIER));
        for (int i = 0; i < newKitchensCount; i++) {
            auto kitchenDetails = createNewKitchen();
            targetKitchens.push_back(std::make_pair(kitchenDetails.IpcAddress,
                                                    kitchenParams.cooksNumber * MAX_ORDERED_PIZZAS_MULTIPLIER));
        }
    }

    distributeOrderedPizzas(targetKitchens);
}

std::vector<std::pair<IpcAddress, KitchenStatusDto>> Reception::closeIdleKitchens(std::vector<std::pair<IpcAddress, KitchenStatusDto>>& statuses) {
    std::set<IpcAddress> idleKitchens;
    std::vector<std::pair<IpcAddress, KitchenStatusDto>> nonIdleKitchenStatuses;

    // Select idle kitchens
    for (const auto &[ipcAddress, status]: statuses) {
        auto now = std::chrono::system_clock::now();
        auto lastActiveTime = std::chrono::system_clock::from_time_t(status.lastActiveTimeSinceEpoch);
        auto idleDuration = std::chrono::duration_cast<std::chrono::seconds>(lastActiveTime - now).count();

        if (idleDuration > KITCHEN_IDLE_TIME_LIMIT_IN_SECONDS && status.queuedPizzaNumber == 0) {
            idleKitchens.insert(ipcAddress);
        }
    }

    std::cerr << "BIBA " << idleKitchens.size() << "\n";

    for (auto pair : statuses) {
        if (idleKitchens.find(pair.first) != idleKitchens.end()) {
            // Close idle kitchen
            std::ostringstream infoMessage;
            infoMessage << "Closing idle kitchen with IPC address: " << pair.first;
            logger->logInfo(infoMessage.str());
            closeKitchen(pair.first);
        }
        else {
            // Add non-idle kitchen
            nonIdleKitchenStatuses.push_back(pair);
        }
    }

    return nonIdleKitchenStatuses;
}

void Reception::handleExecutedOrder(OrderId orderId) {
    auto order = ordersById[orderId];
    ordersById.erase(orderId);
    pizzasLeftToCookByOrder.erase(orderId);

    auto orderString = (*unparsedOrders)[orderId];
    std::cout << "Order is ready: " << orderString << std::endl;
}

std::string Reception::getStatus() {
    auto statuses = pollKitchens();
    auto nonIdleStatuses =  closeIdleKitchens(statuses);

    std::ostringstream oss;

    oss << "Total kitchens: " << static_cast<int>(kitchenPool.size()) << '\n';

    if (nonIdleStatuses.empty()) {
        oss << "None of kitchens respond to polling\n";
        return oss.str();
    }

    for (const auto &[ipcAddress, status]: nonIdleStatuses) {
        std::chrono::system_clock::duration duration(status.lastActiveTimeSinceEpoch);
        std::chrono::system_clock::time_point lastActiveTime(duration);

        oss << "Status of kitchen: " << ipcAddress << ";\t";
        oss << "Available cooks: " << status.availableCookNumber << " of " << status.totalCookNumber << ";\t";
        oss << "Queued pizza number: " << status.queuedPizzaNumber << ";\t";
        oss << "Last activity time: " << timePointToString(lastActiveTime) << ";\t";
        oss << '\n';
    }

    return oss.str();
}
