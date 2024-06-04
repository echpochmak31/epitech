#ifndef PLAZZA_PROJECT_RECEPTION_HPP
#define PLAZZA_PROJECT_RECEPTION_HPP

#include <atomic>
#include <memory>
#include <vector>
#include <string>

#include "Constants.hpp"
#include "KitchenDetails.hpp"
#include "KitchenParams.hpp"
#include "Logger.hpp"
#include "Order.hpp"
#include "ThreadSafeQueue.hpp"
#include "ipc/IMessageBus.hpp"
#include "ipc/Payloads.hpp"

typedef int OrderId;
typedef std::string IpcAddress;

class Reception {
private:
    static size_t totalKitchensCreated;

    std::condition_variable cv;

    std::mutex kitchenPoolMtx;
    std::mutex ordersByIdMtx;
    std::mutex pizzasLeftToCookByOrderMtx;

    std::atomic<bool> receptionEnabled;
    std::shared_ptr<Logger> logger;

    KitchenParams kitchenParams;
    std::string receptionIpcAddress;
    std::shared_ptr<IMessageBus> messageBus;
    std::vector<KitchenDetails> kitchenPool;

    std::shared_ptr<ThreadSafeQueue<Order> > queuedOrders; // thread A
    std::shared_ptr<ThreadSafeQueue<OrderedPizzaDto> > queuedPizzas; // thread A, thread B

    std::unordered_map<OrderId, Order> ordersById; // thread A, thread C
    std::unordered_map<OrderId, int> pizzasLeftToCookByOrder; // thread A, thread C
    std::shared_ptr<ThreadSafeQueue<Order>> executedOrders; // thread C

    void decomposeOrder(const Order &order) const; // thread A

    std::vector<std::pair<IpcAddress, KitchenStatusDto> > pollKitchens(); // thread B

    void distributeOrderedPizzas(std::vector<std::pair<IpcAddress, int> > targetKitchens); // thread B

    void assignPizza(IpcAddress kitchenIpcAddress, OrderedPizzaDto orderedPizzaDto); // thread B

    // add it to pool, e.t.c.
    KitchenDetails createNewKitchen(); // thread B

    void closeKitchen(IpcAddress kitchenIpcAddress); // thread B

    void bindMessageHandlers();

public:
    Reception(std::shared_ptr<ThreadSafeQueue<Order> > queuedOrders, KitchenParams kitchenParams,
              std::shared_ptr<IMessageBus> messageBus,
              std::string ipcAddress = RECEPTION_DEFAULT_IPC_ADDRESS);

    ~Reception();

    void runOrderHandling(); // thread A
    void runOrderedPizzasAssigning(); // thread B
    void runOrderCheck(); // thread C
};

#endif //PLAZZA_PROJECT_RECEPTION_HPP
