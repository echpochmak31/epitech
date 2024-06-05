#include "Utils.hpp"

std::string timePointToString(const std::chrono::system_clock::time_point &tp) {
  std::time_t time = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::localtime(&time);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

std::string makeKitchenIpcAddress(size_t kitchenNumber) {
  std::ostringstream oss;
  oss << "kitchen_id_" << static_cast<int>(kitchenNumber);
  return oss.str();
}

std::string encodePizzaDetails(const PizzaType type, const PizzaSize size) {
  std::ostringstream oss;
  oss << static_cast<int>(type) << ":" << static_cast<int>(size);
  return oss.str();
}
