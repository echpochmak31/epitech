#include "Reception.hpp"
#include "Kitchen.hpp"
#include <sstream>
#include <iostream>
#include <unistd.h> // read, write, pipe, fork
#include <sys/wait.h>
#include <stdexcept>

Reception::Reception(int cooks, int replenish, float multiplier)
    : numCooks(cooks), replenishTime(replenish), cookTimeMultiplier(multiplier) {
    createNewKitchen();
}

void Reception::createNewKitchen() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid == 0) { // Child process
        close(pipefd[0]); // Close the read end
        Kitchen kitchen(numCooks, replenishTime);
        kitchen.run(pipefd[1]); // Pass the write end of the pipe to the kitchen
        close(pipefd[1]); // Close the write end after running the kitchen
        exit(0); // Exit child process after kitchen run
    } else { // Parent process
        close(pipefd[1]); // Close the write end
        kitchenPIDs.push_back(pid);
        kitchenPipes.push_back(pipefd[0]); // Store the read end for communication
    }
}

void Reception::handleOrder(const std::string& order) {
    try {
        std::vector<Pizza> pizzas = parseOrder(order);
        for (const auto& pizza : pizzas) {
            // todo
            // send pizza order to a kitchen process
            // placeholder for sending the order to kitchens
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
        write(kitchenPipes[i], "status\n", 7); // request status from the kitchen
        char buffer[256];
        ssize_t bytesRead = read(kitchenPipes[i], buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer;
        } else {
            std::cerr << "Failed to read status from kitchen " << i + 1 << "\n";
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
