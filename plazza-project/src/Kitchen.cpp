#include "Kitchen.hpp"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>

Kitchen::Kitchen(int numCooks, int replenishTime)
    : stock(replenishTime), maxPizzaCapacity(2 * numCooks), isActive(true) {
    for (int i = 0; i < numCooks; ++i) {
        cooks.push_back(Cook());
    }
    replenishThread = std::thread(&Kitchen::replenishStock, this);
}

Kitchen::~Kitchen() {
    isActive = false;
    if (replenishThread.joinable()) {
        replenishThread.join();
    }
}

bool Kitchen::assignPizza(Pizza pizza) {
    for (auto& cook : cooks) {
        if (!cook.cookThread.joinable()) {
            cook.startCooking(pizza, stock, 1.0); // todo: remove hardcode
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
    char buffer[256];
    while (isActive) {
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1); // Read commands from the pipe
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string command(buffer);
            if (command == "status\n") {
                std::string status = getStatus();
                write(writeFd, status.c_str(), status.size()); // Write status to the pipe
            } else {
                std::vector<Pizza> pizzas = parseOrder(command);
                for (const auto& pizza : pizzas) {
                    if (!assignPizza(pizza)) {
                        std::cerr << "Error: No available cooks to handle the order." << std::endl;
                    }
                }
            }
        } else {
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
