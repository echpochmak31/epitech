#include "ipc/Payloads.hpp"
#include <sstream>

std::string KitchenStatusDto::serialize() const {
    std::ostringstream oss;
    oss << available << ":"
        << availableCookNumber << ":"
        << totalCookNumber << ":"
        << std::chrono::duration_cast<std::chrono::seconds>(updateTime.time_since_epoch()).count() << ":"
        << queuedPizzaNumber << ":";
    return oss.str();
}

KitchenStatusDto KitchenStatusDto::deserialize(std::string &data) {
    std::istringstream iss(data);
    std::string availableStr, availableCookNumberStr, totalCookNumberStr, updateTimeStr, queuedPizzaNumberStr;

    std::getline(iss, availableStr, ':');
    std::getline(iss, availableCookNumberStr, ':');
    std::getline(iss, totalCookNumberStr, ':');
    std::getline(iss, updateTimeStr, ':');
    std::getline(iss, queuedPizzaNumberStr);

    auto result = KitchenStatusDto();
    result.available = (availableStr == "1");
    result.availableCookNumber = std::stoi(availableCookNumberStr);
    result.totalCookNumber = std::stoi(totalCookNumberStr);
    result.updateTime = std::chrono::system_clock::time_point(std::chrono::seconds(std::stoll(updateTimeStr)));
    result.queuedPizzaNumber = std::stoi(queuedPizzaNumberStr);

    return result;
}



std::string OrderedPizzaDto::serialize() const {
    std::ostringstream oss;
    oss << orderId << ":"
        << static_cast<int>(type) << ":"
        << static_cast<int>(size);
    return oss.str();
}


OrderedPizzaDto OrderedPizzaDto::deserialize(std::string &data) {
    std::istringstream iss(data);
    std::string orderIdStr, typeStr, sizeStr;

    std::getline(iss, orderIdStr, ':');
    std::getline(iss, typeStr, ':');
    std::getline(iss, sizeStr);

    auto result = OrderedPizzaDto();
    result.orderId = std::stoi(orderIdStr);
    result.type = static_cast<PizzaType>(std::stoi(typeStr));
    result.size = static_cast<PizzaSize>(std::stoi(sizeStr));

    return result;
}


