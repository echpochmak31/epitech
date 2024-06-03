#ifndef PLAZZA_PROJECT_RECEPTION_HPP
#define PLAZZA_PROJECT_RECEPTION_HPP

#include <atomic>
#include <memory>
#include <vector>
#include <string>

#include "Constants.hpp"
#include "KitchenDetails.hpp"
#include "KitchenParams.hpp"
#include "Order.hpp"
#include "ThreadSafeQueue.hpp"
#include "ipc/IMessageBus.hpp"
#include "ipc/Payloads.hpp"

typedef int OrderId;
typedef std::string IpcAddress;

class Reception {
private:
    std::atomic<bool> receptionEnabled;
    KitchenParams kitchenParams;
    std::vector<KitchenDetails> kitchenPool;
    std::string ipcAddress;
    std::shared_ptr<IMessageBus> messageBus;

    std::shared_ptr<ThreadSafeQueue<Order>>queuedOrders; // thread A
    std::shared_ptr<ThreadSafeQueue<OrderedPizzaDto>> queuedPizzas;  // thread A, thread B

    std::unordered_map<OrderId, Order> orderdById; // thread A
    std::unordered_map<OrderId, int> pizzasLeftToCookByOrder; // thread A, thread D

    void decomposeOrder(Order &order);  // thread A

    std::vector<std::pair<IpcAddress, KitchenStatusDto>> pollKitchens(); // thread B

    void distributeOrderedPizzas(); // thread B

    void assignOrderedPizza(OrderedPizzaDto dto, KitchenDetails &kitchenDetails);

    void bindMessageHandlers();

    void createNewKitchen(); // thread B

public:
    Reception(std::shared_ptr<ThreadSafeQueue<Order>> queuedOrders, KitchenParams kitchenParams, std::shared_ptr<IMessageBus> messageBus,
              std::string ipcAddress = RECEPTION_DEFAULT_IPC_ADDRESS);

    ~Reception();

    void runOrderHandling(); // thread A
    void runOrderedPizzasAssigning(); // thread B
    void listenKitchenSignals(); // thread C
    void runOrderCheck(); // thread D
};

#endif //PLAZZA_PROJECT_RECEPTION_HPP
