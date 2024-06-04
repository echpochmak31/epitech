#ifndef ORDEREDPIZZADTO_HPP
#define ORDEREDPIZZADTO_HPP

#include "PizzaInfo.hpp"
#include <string>
#include <iostream>

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

#endif // ORDEREDPIZZADTO_HPP
