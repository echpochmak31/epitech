#include "Reception.hpp"
#include "Kitchen.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdexcept>

Reception::Reception(int cooks, int replenish, float multiplier)
    : numCooks(cooks), replenishTime(replenish), cookTimeMultiplier(multiplier) {
    createNewKitchen();
}

void Reception::createNewKitchen() {
    auto ipc = std::make_shared<IPC>();
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) { // Child process
        close(ipc->getReadFd()); // Close the read end
        Kitchen kitchen(numCooks, replenishTime);
        kitchen.run(ipc->getWriteFd()); // Pass the write end of the pipe to the kitchen
        close(ipc->getWriteFd()); // Close the write end after running the kitchen
        exit(0); // Exit child process after kitchen run
    } else { // Parent process
        close(ipc->getWriteFd()); // Close the write end
        kitchenPIDs.push_back(pid);
        kitchenPipes.push_back(ipc); // Store the IPC object for communication
    }
}

void Reception::handleOrder(const std::string& order) {
    try {
        std::vector<Pizza> pizzas = parseOrder(order);
        for (const auto& pizza : pizzas) {
            bool assigned = false;
            for (size_t i = 0; i < kitchenPIDs.size(); ++i) {
                try {
                    *kitchenPipes[i] << "assign " + std::to_string(pizza.getType()) + " " +
                                        std::to_string(pizza.getSize()) + " x" +
                                        std::to_string(pizza.getQuantity());
                    std::string response;
                    *kitchenPipes[i] >> response;
                    if (response == "OK") {
                        assigned = true;
                        break;
                    }
                } catch (const std::runtime_error& e) {
                    std::cerr << "Failed to communicate with kitchen " << i + 1 << ": " << e.what() << std::endl;
                }
            }
            if (!assigned) {
                createNewKitchen();
                try {
                    *kitchenPipes.back() << "assign " + std::to_string(pizza.getType()) + " " +
                                           std::to_string(pizza.getSize()) + " x" +
                                           std::to_string(pizza.getQuantity());
                } catch (const std::runtime_error& e) {
                    std::cerr << "Failed to communicate with new kitchen: " << e.what() << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid order format: " << e.what() << std::endl;
    }
}

std::vector<Pizza> Reception::parseOrder(const std::string& order) {
    std::vector<Pizza> pizzas;
    std::istringstream ss(order);
    std::string token;
    while (std::getline(ss, token, ';')) {
        std::istringstream tokenStream(token);
        std::string type, size, quantity;
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

PizzaType Reception::getPizzaType(const std::string& type) {
    if (type == "Regina") return Regina;
    if (type == "Margarita") return Margarita;
    if (type == "Americana") return Americana;
    if (type == "Fantasia") return Fantasia;
    throw std::invalid_argument("Unknown pizza type: " + type);
}

PizzaSize Reception::getPizzaSize(const std::string& size) {
    if (size == "S") return S;
    if (size == "M") return M;
    if (size == "L") return L;
    if (size == "XL") return XL;
    if (size == "XXL") return XXL;
    throw std::invalid_argument("Unknown pizza size: " + size);
}

void Reception::printStatus() {
    for (size_t i = 0; i < kitchenPIDs.size(); ++i) {
        std::cout << "Kitchen process " << i + 1 << " (PID: " << kitchenPIDs[i] << ")\n";
        try {
            *kitchenPipes[i] << "status";
            std::string status;
            *kitchenPipes[i] >> status;
            std::cout << status;
        } catch (const std::runtime_error& e) {
            std::cerr << "Failed to communicate with kitchen " << i + 1 << ": " << e.what() << std::endl;
        }
    }
}

void Reception::handleCommands(int readFd) {
    char buffer[256];
    ssize_t bytesRead;
    while ((bytesRead = read(readFd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        std::string command(buffer);
        if (command == "status\n") {
            printStatus();
        } else if (command == "exit\n") {
            break;
        } else {
            handleOrder(command);
        }
    }
}
