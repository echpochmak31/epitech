#include <sstream>
#include <iostream>
#include "Reception.hpp"


Reception::Reception(int cooks, int replenish, float multiplier)
        : numCooks(cooks), replenishTime(replenish), cookTimeMultiplier(multiplier) {
    createNewKitchen();
}

void Reception::createNewKitchen() {
    kitchens.push_back(std::make_shared<Kitchen>(numCooks, replenishTime));
}

void Reception::handleOrder(const std::string& order) {
    std::vector<Pizza> pizzas = parseOrder(order);
    for (const auto& pizza : pizzas) {
        bool assigned = false;
        for (const auto& kitchen : kitchens) {
            if (kitchen->assignPizza(pizza)) {
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            createNewKitchen();
            kitchens.back()->assignPizza(pizza);
        }
    }
}

std::vector<Pizza> Reception::parseOrder(const std::string& order) {
    std::vector<Pizza> pizzas;
    std::istringstream ss(order);
    std::string token;
    while (std::getline(ss, token, ';')) {
        std::istringstream tokenStream(token);
        std::string type;
        std::string size;
        std::string quantity;
        tokenStream >> type >> size >> quantity;
        PizzaType pizzaType = getPizzaType(type);
        PizzaSize pizzaSize = getPizzaSize(size);
        int qty = std::stoi(quantity.substr(1));
        pizzas.push_back(Pizza(pizzaType, pizzaSize, qty));
    }
    return pizzas;
}

PizzaType Reception::getPizzaType(const std::string& type) {
    if (type == "Regina") return Regina;
    if (type == "Margarita") return Margarita;
    if (type == "Americana") return Americana;
    if (type == "Fantasia") return Fantasia;
    return Margarita;  // default
}

PizzaSize Reception::getPizzaSize(const std::string& size) {
    if (size == "S") return S;
    if (size == "M") return M;
    if (size == "L") return L;
    if (size == "XL") return XL;
    if (size == "XXL") return XXL;
    return M;  // default
}

std::vector<std::string> Reception::getKitchenStatuses() {
    std::vector<std::string> statuses(kitchens.size());
    for (int i = 0; i < kitchens.size(); i++) {
        statuses[i] = kitchens[i]->getStatus();
    }
    return statuses;
}

