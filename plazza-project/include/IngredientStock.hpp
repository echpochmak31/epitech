#ifndef PLAZZA_PROJECT_INGREDIENTSTOCK_HPP
#define PLAZZA_PROJECT_INGREDIENTSTOCK_HPP

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <atomic>

#include "Ingredients.hpp"

class IngredientStock {
private:
    std::mutex stockMutex;
    std::map<Ingredients, int> stock;
    int replenishTimeInMilliseconds;
    std::atomic<bool> running;
    std::thread replenishThread;

    void replenish();

public:
    explicit IngredientStock(int replenishTimeInMilliseconds);
    ~IngredientStock();

    void replenishStock();
    bool useIngredients(const std::set<Ingredients>& ingredients);
    std::string getStatus();
};

#endif //PLAZZA_PROJECT_INGREDIENTSTOCK_HPP
