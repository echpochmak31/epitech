#ifndef PLAZZA_PROJECT_RECEPTION_HPP
#define PLAZZA_PROJECT_RECEPTION_HPP

#include <vector>
#include <string>
#include <memory>
#include "Pizza.hpp"
#include "Kitchen.hpp"

class Reception {
private:
    std::vector<std::shared_ptr<Kitchen>> kitchens;
    int numCooks;
    int replenishTime;
    float cookTimeMultiplier;

public:
    Reception(int cooks, int replenish, float multiplier);
    void createNewKitchen();
    void handleOrder(const std::string& order);
    std::vector<Pizza> parseOrder(const std::string& order);
    PizzaType getPizzaType(const std::string& type);
    PizzaSize getPizzaSize(const std::string& size);
    std::vector<std::string> getKitchenStatuses();
};


#endif //PLAZZA_PROJECT_RECEPTION_HPP
