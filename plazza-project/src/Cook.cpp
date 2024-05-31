#include <chrono>
#include <thread>
#include "Cook.hpp"

Cook::Cook() = default;

void Cook::startCooking(Pizza pizza, IngredientStock& stock, float cookTimeMultiplier) {
    cookThread = std::thread(&Cook::cookPizza, this, pizza, std::ref(stock), cookTimeMultiplier);
}

void Cook::cookPizza(Pizza pizza, IngredientStock& stock, float cookTimeMultiplier) {
    std::vector<std::string> ingredients;
    switch (pizza.type) {
        case Margarita:
            ingredients = {"dough", "tomato", "gruyere"};
            break;
        case Regina:
            ingredients = {"dough", "tomato", "gruyere", "ham", "mushrooms"};
            break;
        case Americana:
            ingredients = {"dough", "tomato", "gruyere", "steak"};
            break;
        case Fantasia:
            ingredients = {"dough", "tomato", "eggplant", "goat_cheese", "chief_love"};
            break;
    }
    if (stock.useIngredients(ingredients)) {
        int cookTime = getCookTime(pizza.type, cookTimeMultiplier);
        std::this_thread::sleep_for(std::chrono::milliseconds(cookTime));
    }
}

int Cook::getCookTime(PizzaType type, float cookTimeMultiplier) {
    int baseTime = 0;
    switch (type) {
        case Margarita:
            baseTime = 1000;
            break;
        case Regina:
            baseTime = 2000;
            break;
        case Americana:
            baseTime = 2000;
            break;
        case Fantasia:
            baseTime = 4000;
            break;
    }
    return static_cast<int>(baseTime * cookTimeMultiplier);
}
