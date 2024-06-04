#ifndef UTILS_HPP
#define UTILS_HPP
#include <chrono>
#include <iomanip>
#include <PizzaInfo.hpp>
#include <sstream>

std::string timePointToString(const std::chrono::system_clock::time_point &tp);

std::string makeKitchenIpcAddress(size_t kitchenNumber);

std::string encodePizzaDetails(PizzaType type, PizzaSize size);

#endif //UTILS_HPP
