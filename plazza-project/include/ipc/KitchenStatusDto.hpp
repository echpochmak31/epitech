#ifndef KITCHENSTATUSDTO_HPP
#define KITCHENSTATUSDTO_HPP

#include <string>

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

#endif // KITCHENSTATUSDTO_HPP
