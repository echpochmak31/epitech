#include "IngredientStock.hpp"
#include <thread>
#include <sstream>

IngredientStock::IngredientStock(int replenishTimeMs) : replenishTime(replenishTimeMs) {
    stock["dough"] = 5;
    stock["tomato"] = 5;
    stock["gruyere"] = 5;
    stock["ham"] = 5;
    stock["mushrooms"] = 5;
    stock["steak"] = 5;
    stock["eggplant"] = 5;
    stock["goat_cheese"] = 5;
    stock["chief_love"] = 5;
}

void IngredientStock::replenishStock() {
    std::this_thread::sleep_for(std::chrono::milliseconds(replenishTime));
    std::lock_guard<std::mutex> lock(stockMutex);
    for (auto &ingredient : stock) {
        ingredient.second += 1;
    }
}

bool IngredientStock::useIngredients(const std::vector<std::string>& ingredients) {
    std::lock_guard<std::mutex> lock(stockMutex);
    for (const auto& ingredient : ingredients) {
        if (stock[ingredient] <= 0) {
            return false;
        }
    }
    for (const auto& ingredient : ingredients) {
        stock[ingredient] -= 1;
    }
    return true;
}

std::string IngredientStock::getStatus() {
    std::ostringstream status;
    std::lock_guard<std::mutex> lock(stockMutex);
    for (const auto& ingredient : stock) {
        status << "  " << ingredient.first << ": " << ingredient.second << "\n";
    }
    return status.str();
}
