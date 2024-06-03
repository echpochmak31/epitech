#include "Reception.hpp"
#include "KitchenDetails.hpp"

// todo: refactor constructor
Reception::Reception(ThreadSafeQueue<Order> &queuedOrders, KitchenParams kitchenParams,
                     std::shared_ptr<IMessageBus> messageBus, std::string ipcAddress) {
    receptionEnabled = true;
}

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
    // todo
}


void Reception::distributeOrderedPizzas() {
    // todo
}

void Reception::assignOrderedPizza(OrderedPizzaDto dto, KitchenDetails &kitchenDetails) {
    // todo
}

void Reception::bindMessageHandlers() {
    // todo
}

void Reception::createNewKitchen() {
    // todo
}

// thread A
void Reception::runOrderedPizzasAssigning() {
    std::vector<std::pair<IpcAddress, int> > targetKitchens; // address - number of pizzas ready to accept

    while (receptionEnabled) {
        pizzasLeftToCookByOrder.clear();
        int totalPizzaCountReadyToAccept = 0;
        int fixedQueuedPizzasSize = queuedPizzas->size();

        auto statuses = pollKitchens();

        for (const auto &[ipcAddress, status]: statuses) {
            if (status.available) {
                int count = status.totalCookNumber * MAX_ORDERED_PIZZAS_MULTIPLIER - status.queuedPizzaNumber;
                totalPizzaCountReadyToAccept += count;
                targetKitchens.push_back(std::make_pair(ipcAddress, count));
            }
        }

        int deficiency = fixedQueuedPizzasSize - totalPizzaCountReadyToAccept;

        if (deficiency > 0) {
            // todo: allocate more kitchens
            // todo: add new kitchens to targetKitchens
        }

        // todo: distribute
    }
}

void Reception::runOrderHandling() {
    while (receptionEnabled) {
        if (queuedOrders->empty())
            continue;

        auto order = queuedOrders->dequeue();
        orderdById[order.id] = order;
        pizzasLeftToCookByOrder[order.id] = order.totalPizzaNumber;
        decomposeOrder(order);
    }
}
