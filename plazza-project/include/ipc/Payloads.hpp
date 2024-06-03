#ifndef PAYLOADS_HPP
#define PAYLOADS_HPP
#include <chrono>
#include <string>
#include "PizzaInfo.hpp"

class KitchenStatusDto {
public:
    bool available;
    int availableCookNumber;
    int totalCookNumber;
    int queuedPizzaNumber;
    std::chrono::system_clock::time_point updateTime;

    std::string serialize() const;
    static KitchenStatusDto deserialize(std::string& data);
};


class OrderedPizzaDto {
public:
    int orderId;
    PizzaType type;
    PizzaSize size;

    std::string serialize() const;
    static OrderedPizzaDto deserialize(std::string& data);
};

#endif //PAYLOADS_HPP
