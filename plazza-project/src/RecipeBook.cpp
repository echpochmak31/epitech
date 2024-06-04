#include "RecipeBook.hpp"
#include <cmath>

float RecipeBook::CookingTimeMultiplier = 1.0;

std::map<PizzaType, int> RecipeBook::bakingTimes = {
    {Margarita, 1000},
    {Regina, 2000},
    {Americana, 2000},
    {Fantasia, 4000},
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

std::mutex RecipeBook::recipeMutex;

int RecipeBook::getCookingTimeInMilliseconds(PizzaType pizzaType, PizzaSize pizzaSize) {
    std::lock_guard<std::mutex> lock(recipeMutex);
    return bakingTimes[pizzaType];
}

std::set<Ingredients> RecipeBook::getRecipe(PizzaType pizzaType) {
    std::lock_guard<std::mutex> lock(recipeMutex);
    return Recipes[pizzaType];
}
