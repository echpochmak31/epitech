#ifndef PIZZAINFO_HPP
#define PIZZAINFO_HPP

#include <unordered_map>
#include <stdexcept>


enum PizzaType {
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8
};

enum PizzaSize {
    S = 1,
    M = 2,
    L = 4,
    XL = 8,
    XXL = 16
};

#endif //PIZZAINFO_HPP
