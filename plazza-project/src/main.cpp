#include <iostream>
#include "Reception.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <cooking_time_multiplier> <num_cooks> <replenish_time>" << std::endl;
        return 1;
    }

    float cookingTimeMultiplier = std::stof(argv[1]);
    int numCooks = std::stoi(argv[2]);
    int replenishTime = std::stoi(argv[3]);

    Reception reception(numCooks, replenishTime, cookingTimeMultiplier);

    std::string command;
    while (true) {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);
        if (command == "exit") {
            break;
        } else if (command == "status") {
            auto statuses = reception.getKitchenStatuses();
            for (const auto& status : statuses) {
                std::cout << status << "\n";
            }
        } else {
            reception.handleOrder(command);
        }
    }

    return 0;
}
