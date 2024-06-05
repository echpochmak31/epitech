#include "ipc/KitchenStatusDto.hpp"

#include <sstream>

std::string KitchenStatusDto::serialize() const {
  std::ostringstream oss;
  oss << available << ":" << availableCookNumber << ":" << totalCookNumber
      << ":" << updateTimeSinceEpoch << ":" << queuedPizzaNumber << ":"
      << lastActiveTimeSinceEpoch;
  return oss.str();
}

KitchenStatusDto KitchenStatusDto::deserialize(std::string &data) {
  std::istringstream iss(data);
  std::string availableStr, availableCookNumberStr, totalCookNumberStr,
      updateTimeStr, queuedPizzaNumberStr, lastActiveTimeStr;

  std::getline(iss, availableStr, ':');
  std::getline(iss, availableCookNumberStr, ':');
  std::getline(iss, totalCookNumberStr, ':');
  std::getline(iss, updateTimeStr, ':');
  std::getline(iss, queuedPizzaNumberStr, ':');
  std::getline(iss, lastActiveTimeStr);

  auto result = KitchenStatusDto();
  result.available = (availableStr == "1");
  result.availableCookNumber = std::stoi(availableCookNumberStr);
  result.totalCookNumber = std::stoi(totalCookNumberStr);
  result.updateTimeSinceEpoch = std::stol(updateTimeStr);
  result.queuedPizzaNumber = std::stoi(queuedPizzaNumberStr);
  result.lastActiveTimeSinceEpoch = std::stol(lastActiveTimeStr);

  return result;
}
