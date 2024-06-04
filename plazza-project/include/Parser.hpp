#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "Order.hpp"
#include "PizzaInfo.hpp"

class Parser {
public:
    static PizzaType stringToPizzaType(const std::string& str);
    static PizzaSize stringToPizzaSize(const std::string& str);
    static Order parseOrder(const std::string& input);
};

#endif //PARSER_HPP
