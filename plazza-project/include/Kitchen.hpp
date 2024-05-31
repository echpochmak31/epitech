#ifndef PLAZZA_PROJECT_KITCHEN_HPP
#define PLAZZA_PROJECT_KITCHEN_HPP

#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include "Cook.hpp"
#include "IngredientStock.hpp"
#include "Pizza.hpp"
#include "IPC.hpp"

class Kitchen {
private:
    std::vector<Cook> cooks;
    IngredientStock stock;
    IPC ipc;
    int maxPizzaCapacity;
    bool isActive;
    std::thread replenishThread;
    std::chrono::steady_clock::time_point lastActiveTime;

public:
    Kitchen(int numCooks, int replenishTime);
    ~Kitchen();
    bool assignPizza(Pizza pizza);
    void replenishStock();
    void run(int writeFd); // Pass the write end of the pipe to the kitchen
    void handleMessages();
    std::string getStatus();
    std::vector<Pizza> parseOrder(const std::string& message);
    PizzaType getPizzaType(const std::string& type);
    PizzaSize getPizzaSize(const std::string& size);
};

#endif //PLAZZA_PROJECT_KITCHEN_HPP
