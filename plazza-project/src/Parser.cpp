#include "Parser.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include "Utils.hpp"

PizzaType Parser::stringToPizzaType(const std::string& str) {
  std::string upperStr = str;
  std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

  if (upperStr == "REGINA")
    return PizzaType::Regina;
  else if (upperStr == "MARGARITA")
    return PizzaType::Margarita;
  else if (upperStr == "AMERICANA")
    return PizzaType::Americana;
  else if (upperStr == "FANTASIA")
    return PizzaType::Fantasia;
  else
    throw std::invalid_argument("Invalid pizza type: " + str);
}

PizzaSize Parser::stringToPizzaSize(const std::string& str) {
  std::string upperStr = str;
  std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);

  if (upperStr == "S")
    return PizzaSize::S;
  else if (upperStr == "M")
    return PizzaSize::M;
  else if (upperStr == "L")
    return PizzaSize::L;
  else if (upperStr == "XL")
    return PizzaSize::XL;
  else if (upperStr == "XXL")
    return PizzaSize::XXL;
  else
    throw std::invalid_argument("Invalid pizza size: " + str);
}

Order Parser::parseOrder(const std::string& input) {
  std::unordered_map<std::string, PizzaType> pizzaTypeMap = {
      {"regina", PizzaType::Regina},
      {"margarita", PizzaType::Margarita},
      {"americana", PizzaType::Americana},
      {"fantasia", PizzaType::Fantasia}};

  std::unordered_map<std::string, PizzaSize> pizzaSizeMap = {
      {"S", PizzaSize::S},
      {"M", PizzaSize::M},
      {"L", PizzaSize::L},
      {"XL", PizzaSize::XL},
      {"XXL", PizzaSize::XXL}};

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
