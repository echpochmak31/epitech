#ifndef RECIPEBOOK_HPP
#define RECIPEBOOK_HPP
#include <map>
#include <set>
#include <PizzaInfo.hpp>
#include "Ingredients.hpp"

class RecipeBook {
private:
    static std::map<PizzaType, int> bakingTimes;
public:
    static std::map<PizzaType, std::set<Ingredients>> Recipes;
    static float CookingTimeMultiplier;
    static int getCookingTimeInMilliseconds(PizzaType pizzaType, PizzaSize pizzaSize);
};


#endif //RECIPEBOOK_HPP
