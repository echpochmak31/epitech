#ifndef PLAZZA_PROJECT_KITCHEN_HPP
#define PLAZZA_PROJECT_KITCHEN_HPP

#include <vector>
#include <memory>
#include <thread>
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

public:
    Kitchen(int numCooks, int replenishTime);
    ~Kitchen();
    bool assignPizza(Pizza pizza);
    void replenishStock();
    void run();
    std::string getStatus();
    std::vector<Pizza> parseOrder(const std::string& message); // New method to parse orders
    PizzaType getPizzaType(const std::string& type); // Helper method to get PizzaType from string
    PizzaSize getPizzaSize(const std::string& size); // Helper method to get PizzaSize from string
};

#endif //PLAZZA_PROJECT_KITCHEN_HPP
