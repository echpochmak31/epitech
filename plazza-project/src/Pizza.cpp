#include "Pizza.hpp"

Pizza::Pizza(PizzaType t, PizzaSize s, int q) : type(t), size(s), quantity(q) {}

PizzaType Pizza::getType() const {
    return type;
}

PizzaSize Pizza::getSize() const {
    return size;
}

int Pizza::getQuantity() const {
    return quantity;
}
