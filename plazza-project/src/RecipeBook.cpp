#include "RecipeBook.hpp"
#include <cmath>

float RecipeBook::CookingTimeMultiplier = 1.0;

std::map<PizzaType, int> RecipeBook::bakingTimes = {
    {Margarita, 1},
    {Regina, 2},
    {Americana, 2},
    {Fantasia, 4},
};


std::map<PizzaType, std::set<Ingredients> > RecipeBook::Recipes = {
    {
        Margarita,
        {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere}
    },
    {
        Regina,
        {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere, Ingredients::Ham, Ingredients::Mushrooms}
    },
    {
        Americana,
        {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere, Ingredients::Steak}
    },
    {
        Fantasia,
        {
            Ingredients::Dough, Ingredients::Tomato, Ingredients::Eggplant, Ingredients::GoatCheese,
            Ingredients::ChiefLove
        }
    },
};


int RecipeBook::getCookingTimeInMilliseconds(PizzaType pizzaType, PizzaSize pizzaSize) {
    auto timeInMilliseconds = static_cast<float>(bakingTimes[pizzaType]) * CookingTimeMultiplier;
    return static_cast<int>(std::round(timeInMilliseconds));
}
