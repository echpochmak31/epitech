#include "ipc/Payloads.hpp"
#include <sstream>

std::string KitchenStatusDto::serialize() const {
    std::ostringstream oss;
    oss << available << ":"
        << availableCookNumber << ":"
        << totalCookNumber << ":"
        << std::chrono::duration_cast<std::chrono::seconds>(updateTime.time_since_epoch()).count();
    return oss.str();
}

KitchenStatusDto KitchenStatusDto::deserialize(std::string &data) {
    std::istringstream iss(data);
    std::string availableStr, availableCookNumberStr, totalCookNumberStr, updateTimeStr;

    std::getline(iss, availableStr, ':');
    std::getline(iss, availableCookNumberStr, ':');
    std::getline(iss, totalCookNumberStr, ':');
    std::getline(iss, updateTimeStr);

    auto result = KitchenStatusDto();
    result.available = (availableStr == "1");
    result.availableCookNumber = std::stoi(availableCookNumberStr);
    result.totalCookNumber = std::stoi(totalCookNumberStr);
    result.updateTime = std::chrono::system_clock::time_point(std::chrono::seconds(std::stoll(updateTimeStr)));

    return result;
}

