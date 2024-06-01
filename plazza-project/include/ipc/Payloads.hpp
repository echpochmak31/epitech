#ifndef PAYLOADS_HPP
#define PAYLOADS_HPP
#include <chrono>
#include <string>

class KitchenStatusDto {
public:
    bool available;
    int availableCookNumber;
    int totalCookNumber;
    std::chrono::system_clock::time_point updateTime;

    std::string serialize() const;
    static KitchenStatusDto deserialize(std::string& data);
};

#endif //PAYLOADS_HPP
