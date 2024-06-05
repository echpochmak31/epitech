#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "Parser.hpp"
#include "Reception.hpp"
#include "RecipeBook.hpp"
#include "Utils.hpp"
#include "ipc/KernelQueueMessageBus.hpp"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <cooking_time_multiplier> <num_cooks> <replenish_time>"
              << std::endl;
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

  std::shared_ptr<IMessageBus> messageBus =
      std::make_shared<KernelQueueMessageBus>(logger);

  auto queuedOrders = std::make_shared<ThreadSafeQueue<Order>>();

  logger->logInfo("Let's go!");

  auto unparsedOrders =
      std::make_shared<std::unordered_map<OrderId, std::string>>();

  auto reception = std::make_shared<Reception>(
      logger, queuedOrders, kitchenParams, messageBus, unparsedOrders);

  std::thread receptionThread(&Reception::run, reception);
  receptionThread.detach();

  std::thread handlerThread(&IMessageBus::runMessageHandling, messageBus.get());
  handlerThread.detach();

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
        Order order = Parser::parseOrder(input);
        (*unparsedOrders)[order.id] = input;
        queuedOrders->enqueue(order);
      } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }
    }
  }

  reception->stop();
  messageBus->dispose();

  return 0;
}
