#ifndef PLAZZA_PROJECT_RECEPTION_HPP
#define PLAZZA_PROJECT_RECEPTION_HPP

#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <thread>

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

    std::shared_ptr<std::unordered_map<OrderId, std::string> > unparsedOrders;

    std::mutex mtx;
    std::condition_variable cv;

    std::atomic<bool> receptionEnabled;
    std::shared_ptr<Logger> logger;

    KitchenParams kitchenParams;
    std::string receptionIpcAddress;
    std::shared_ptr<IMessageBus> messageBus;
    std::vector<KitchenDetails> kitchenPool;

    std::shared_ptr<ThreadSafeQueue<Order> > queuedOrders;
    std::shared_ptr<ThreadSafeQueue<OrderedPizzaDto> > queuedPizzas;

    std::unordered_map<OrderId, Order> ordersById;
    std::unordered_map<OrderId, int> pizzasLeftToCookByOrder;
    std::shared_ptr<ThreadSafeQueue<Order> > executedOrders;

    void decomposeOrder(const Order &order) const; // thread A

    std::vector<std::pair<IpcAddress, KitchenStatusDto> > pollKitchens();

    void distributeOrderedPizzas(std::vector<std::pair<IpcAddress, int> > targetKitchens);

    void assignPizza(IpcAddress kitchenIpcAddress, OrderedPizzaDto orderedPizzaDto);

    KitchenDetails createNewKitchen();

    void closeKitchen(IpcAddress kitchenIpcAddress);

    void handleOrders();
    void assignPizzas();
    std::vector<std::pair<IpcAddress, KitchenStatusDto> > closeIdleKitchens(
        std::vector<std::pair<IpcAddress, KitchenStatusDto> > &statuses);

    void handleExecutedOrder(OrderId orderId);

public:
    Reception(std::shared_ptr<Logger> logger,
              std::shared_ptr<ThreadSafeQueue<Order> > queuedOrders,
              KitchenParams kitchenParams,
              std::shared_ptr<IMessageBus> messageBus,
              std::shared_ptr<std::unordered_map<OrderId, std::string> > unparsedOrders);

    ~Reception();

    void run();

    void stop();

    std::string getStatus();
};

#endif //PLAZZA_PROJECT_RECEPTION_HPP
