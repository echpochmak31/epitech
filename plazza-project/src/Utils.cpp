#include "Utils.hpp"

std::string timePointToString(const std::chrono::system_clock::time_point &tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string makeKitchenIpcAddress(const pid_t kitchenPid) {
    return "kitchen_pid_" + kitchenPid;
}

std::string encodePizzaDetails(const PizzaType type, const PizzaSize size) {
    std::ostringstream oss;
    oss << static_cast<int>(type) << ":" << static_cast<int>(size);
    return oss.str();
}
