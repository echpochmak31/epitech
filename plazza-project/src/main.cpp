#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "Reception.hpp"
#include "RecipeBook.hpp"
#include "Utils.hpp"
#include "ipc/KernelQueueMessageBus.hpp"

// Helper function to convert string to PizzaType
PizzaType stringToPizzaType(const std::string& str) {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

    if (upperStr == "regina") return PizzaType::Regina;
    else if (upperStr == "margarita") return PizzaType::Margarita;
    else if (upperStr == "americana") return PizzaType::Americana;
    else if (upperStr == "fantasia") return PizzaType::Fantasia;
    else throw std::invalid_argument("Invalid pizza type: " + str);
}

// Helper function to convert string to PizzaSize
PizzaSize stringToPizzaSize(const std::string& str) {
    std::string upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

    if (upperStr == "S") return PizzaSize::S;
    else if (upperStr == "M") return PizzaSize::M;
    else if (upperStr == "L") return PizzaSize::L;
    else if (upperStr == "XL") return PizzaSize::XL;
    else if (upperStr == "XXL") return PizzaSize::XXL;
    else throw std::invalid_argument("Invalid pizza size: " + str);
}

Order parseOrder(const std::string& input) {
    std::unordered_map<std::string, PizzaType> pizzaTypeMap = {
        {"regina", PizzaType::Regina},
        {"margarita", PizzaType::Margarita},
        {"americana", PizzaType::Americana},
        {"fantasia", PizzaType::Fantasia}
    };

    std::unordered_map<std::string, PizzaSize> pizzaSizeMap = {
        {"S", PizzaSize::S},
        {"M", PizzaSize::M},
        {"L", PizzaSize::L},
        {"XL", PizzaSize::XL},
        {"XXL", PizzaSize::XXL}
    };

    Order order;
    order.id = ++orderCounter;
    order.totalPizzaNumber = 0;

    std::istringstream ss(input);
    std::string pizzaOrder;
    while (std::getline(ss, pizzaOrder, ';')) {
        std::istringstream pizzaStream(pizzaOrder);
        pizzaStream >> std::ws;
        std::string typeStr, sizeStr, quantityStr;

        std::getline(pizzaStream, typeStr, ' ');
        std::getline(pizzaStream, sizeStr, ' ');
        std::getline(pizzaStream, quantityStr, ' ');
        int quantity = std::stoi(quantityStr.substr(1));

        std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);
        std::transform(sizeStr.begin(), sizeStr.end(), sizeStr.begin(), ::toupper);

        PizzaType type = pizzaTypeMap[typeStr];
        PizzaSize size = pizzaSizeMap[sizeStr];

        order.types.push_back(type);
        order.sizes.push_back(size);
        order.quantities.push_back(quantity);
        order.totalPizzaNumber += quantity;
    }

    return order;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <cooking_time_multiplier> <num_cooks> <replenish_time>" << std::endl;
        return 1;
    }

    float cookingTimeMultiplier = std::stof(argv[1]);
    int cooksPerKitchen = std::stoi(argv[2]);
    int replenishTimeInMilliseconds = std::stoi(argv[3]);

    RecipeBook::CookingTimeMultiplier = cookingTimeMultiplier;

    auto kitchenParams = KitchenParams();
    kitchenParams.cookingTimeMultiplier = cookingTimeMultiplier;
    kitchenParams.cooksNumber = cooksPerKitchen;
    kitchenParams.stockReplenishTimeInMilliseconds = replenishTimeInMilliseconds;


    // Logging to file

    // std::ofstream logFile("./log.txt");
    //
    // // Ensure the log file is open
    // if (!logFile.is_open()) {
    //     std::cerr << "Failed to open log file." << std::endl;
    //     return 1;
    // }
    // auto logger = std::make_shared<Logger>(logFile);

    // Logging to stdout
    auto logger = std::make_shared<Logger>(std::cout);

    std::shared_ptr<IMessageBus> messageBus = std::make_shared<KernelQueueMessageBus>(logger);

    auto queuedOrders = std::make_shared<ThreadSafeQueue<Order> >();

    logger->logInfo("Let's go!");

    auto unparsedOrders = std::make_shared<std::unordered_map<OrderId, std::string>>();

    auto reception = std::make_shared<Reception>(logger, queuedOrders, kitchenParams, messageBus, unparsedOrders);
    std::thread receptionThread(&Reception::run, reception);
    // receptionThread.detach();

    std::thread handlerThread(&IMessageBus::runMessageHandling, messageBus.get());
    // handlerThread.detach();

    std::string input;

    while (true) {
        std::cout << "Enter order or 'status' or 'exit': ";
        std::getline(std::cin, input);

        if (input == "status") {
            std::cout << reception->getStatus() << std::endl;
        } else if (input == "exit") {
            break;
        } else {
            try {
                Order order = parseOrder(input);
                (*unparsedOrders)[order.id] = input;
                queuedOrders->enqueue(order);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }

    messageBus->dispose();

    if (handlerThread.joinable()) {
        handlerThread.join();
    }

    if (receptionThread.joinable()) {
        receptionThread.join();
    }

    return 0;
}
