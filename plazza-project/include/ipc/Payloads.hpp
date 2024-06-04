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
    long updateTimeSinceEpoch;
    long lastActiveTimeSinceEpoch;

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

    friend std::ostream& operator<<(std::ostream& os, const OrderedPizzaDto& dto);
    friend std::istream& operator>>(std::istream& is, OrderedPizzaDto& dto);
};

#endif //PAYLOADS_HPP
