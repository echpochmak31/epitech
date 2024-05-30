#ifndef PLAZZA_PROJECT_COOK_HPP
#define PLAZZA_PROJECT_COOK_HPP

#include <thread>
#include <vector>
#include "Pizza.hpp"

#include <thread>
#include "Pizza.hpp"
#include "IngredientStock.hpp"

class Cook {
public:
    std::thread cookThread;

    Cook();
    void startCooking(Pizza pizza, IngredientStock& stock, float cookTimeMultiplier);
    void cookPizza(Pizza pizza, IngredientStock& stock, float cookTimeMultiplier);
    int getCookTime(PizzaType type, float cookTimeMultiplier);
};

#endif //PLAZZA_PROJECT_COOK_HPP
