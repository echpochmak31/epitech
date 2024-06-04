#include "ipc/OrderedPizzaDto.hpp"
#include <sstream>

std::string OrderedPizzaDto::serialize() const {
    std::ostringstream oss;
    oss << orderId << ":"
        << static_cast<int>(type) << ":"
        << static_cast<int>(size);
    return oss.str();
}

OrderedPizzaDto OrderedPizzaDto::deserialize(std::string &data) {
    std::istringstream iss(data);
    std::string orderIdStr, typeStr, sizeStr;

    std::getline(iss, orderIdStr, ':');
    std::getline(iss, typeStr, ':');
    std::getline(iss, sizeStr);

    auto result = OrderedPizzaDto();
    result.orderId = std::stoi(orderIdStr);
    result.type = static_cast<PizzaType>(std::stoi(typeStr));
    result.size = static_cast<PizzaSize>(std::stoi(sizeStr));

    return result;
}

std::ostream& operator<<(std::ostream& os, const OrderedPizzaDto& dto) {
    os << dto.orderId << ' ' << static_cast<int>(dto.type) << ' ' << static_cast<int>(dto.size);
    return os;
}

std::istream& operator>>(std::istream& is, OrderedPizzaDto& dto) {
    int type, size;
    is >> dto.orderId >> type >> size;
    dto.type = static_cast<PizzaType>(type);
    dto.size = static_cast<PizzaSize>(size);
    return is;
}
