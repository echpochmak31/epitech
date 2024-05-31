#include "Kitchen.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stdexcept>

Kitchen::Kitchen(int numCooks, int replenishTime)
    : stock(replenishTime), maxPizzaCapacity(2 * numCooks), isActive(true) {
    for (int i = 0; i < numCooks; ++i) {
        cooks.push_back(Cook());
    }
    replenishThread = std::thread(&Kitchen::replenishStock, this);
    lastActiveTime = std::chrono::steady_clock::now();
}

Kitchen::~Kitchen() {
    isActive = false;
    if (replenishThread.joinable()) {
        replenishThread.join();
    }
}

bool Kitchen::assignPizza(Pizza pizza) {
    int busyCooks = 0;
    for (auto& cook : cooks) {
        if (cook.cookThread.joinable()) {
            ++busyCooks;
        }
    }
    if (busyCooks >= maxPizzaCapacity) {
        return false;
    }

    for (auto& cook : cooks) {
        if (!cook.cookThread.joinable()) {
            cook.startCooking(pizza, stock, 1.0); // todo: remove hardcode
            lastActiveTime = std::chrono::steady_clock::now();
            return true;
        }
    }
    return false;
}

void Kitchen::replenishStock() {
    while (isActive) {
        stock.replenishStock();
    }
}

void Kitchen::run(int writeFd) {
    std::thread messageThread(&Kitchen::handleMessages, this);
    messageThread.join();
}

void Kitchen::handleMessages() {
    while (isActive) {
        std::string message;
        ipc >> message;
        if (message == "shutdown") {
            isActive = false;
        } else if (message == "status") {
            std::string status = getStatus();
            ipc << status;
        } else {
            std::vector<Pizza> pizzas = parseOrder(message);
            for (const auto& pizza : pizzas) {
                if (!assignPizza(pizza)) {
                    std::cerr << "Error: No available cooks to handle the order." << std::endl;
                }
            }
        }

        if (std::chrono::steady_clock::now() - lastActiveTime > std::chrono::seconds(5)) { // todo: remove hardcode
            isActive = false;
        }
    }
}

std::string Kitchen::getStatus() {
    std::ostringstream status;
    status << "Kitchen status:\n";
    status << "Cooks:\n";
    for (size_t i = 0; i < cooks.size(); ++i) {
        status << "  Cook " << i + 1 << ": " << (cooks[i].cookThread.joinable() ? "Busy" : "Available") << "\n";
    }
    status << "Ingredient Stock:\n";
    status << stock.getStatus();

    return status.str();
}

std::vector<Pizza> Kitchen::parseOrder(const std::string& message) {
    std::vector<Pizza> pizzas;
    std::istringstream ss(message);
    std::string token;
    while (std::getline(ss, token, ';')) {
        std::istringstream tokenStream(token);
        std::string type;
        std::string size;
        std::string quantity;
        tokenStream >> type >> size >> quantity;
        if (type.empty() || size.empty() || quantity.empty() || quantity[0] != 'x') {
            throw std::invalid_argument("Order format is invalid");
        }
        PizzaType pizzaType = getPizzaType(type);
        PizzaSize pizzaSize = getPizzaSize(size);
        int qty = std::stoi(quantity.substr(1));
        pizzas.push_back(Pizza(pizzaType, pizzaSize, qty));
    }
    return pizzas;
}

PizzaType Kitchen::getPizzaType(const std::string& type) {
    if (type == "Regina") return Regina;
    if (type == "Margarita") return Margarita;
    if (type == "Americana") return Americana;
    if (type == "Fantasia") return Fantasia;
    return Margarita;  // default
}

PizzaSize Kitchen::getPizzaSize(const std::string& size) {
    if (size == "S") return S;
    if (size == "M") return M;
    if (size == "L") return L;
    if (size == "XL") return XL;
    if (size == "XXL") return XXL;
    return M;  // default
}
