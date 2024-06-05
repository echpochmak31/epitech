#include "RecipeBook.hpp"

#include <cmath>

float RecipeBook::CookingTimeMultiplier = 1.0;

std::map<PizzaType, int> RecipeBook::bakingTimes = {
    {PizzaType::Margarita, 1000},
    {PizzaType::Regina, 2000},
    {PizzaType::Americana, 2000},
    {PizzaType::Fantasia, 4000},
};

std::map<PizzaType, std::set<Ingredients> > RecipeBook::Recipes = {
    {PizzaType::Margarita,
     {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere}},
    {PizzaType::Regina,
     {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere,
      Ingredients::Ham, Ingredients::Mushrooms}},
    {PizzaType::Americana,
     {Ingredients::Dough, Ingredients::Tomato, Ingredients::Gruyere,
      Ingredients::Steak}},
    {PizzaType::Fantasia,
     {Ingredients::Dough, Ingredients::Tomato, Ingredients::Eggplant,
      Ingredients::GoatCheese, Ingredients::ChiefLove}},
};

std::mutex RecipeBook::recipeMutex;

int RecipeBook::getCookingTimeInMilliseconds(PizzaType pizzaType,
                                             PizzaSize pizzaSize) {
  std::lock_guard<std::mutex> lock(recipeMutex);
  return bakingTimes[pizzaType];
}

std::set<Ingredients> RecipeBook::getRecipe(PizzaType pizzaType) {
  std::lock_guard<std::mutex> lock(recipeMutex);
  return Recipes[pizzaType];
}
