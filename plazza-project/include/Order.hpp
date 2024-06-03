#ifndef ORDER_HPP
#define ORDER_HPP
#include "ipc/Payloads.hpp"

class Order {
public:
    int id;
    int totalPizzaNumber;

    std::vector<PizzaType> types;
    std::vector<PizzaSize> sizes;
    std::vector<int> quantities;
};

#endif //ORDER_HPP
