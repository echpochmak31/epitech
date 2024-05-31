#ifndef PLAZZA_PROJECT_INGREDIENTSTOCK_HPP
#define PLAZZA_PROJECT_INGREDIENTSTOCK_HPP

#include <map>
#include <mutex>
#include <string>
#include <vector>

class IngredientStock {
private:
    std::map<std::string, int> stock;
    std::mutex stockMutex;
    int replenishTime;

public:
    IngredientStock(int replenishTimeMs);
    void replenishStock();
    bool useIngredients(const std::vector<std::string>& ingredients);
    std::string getStatus();
    int getReplenishTime() const;
};

#endif //PLAZZA_PROJECT_INGREDIENTSTOCK_HPP
