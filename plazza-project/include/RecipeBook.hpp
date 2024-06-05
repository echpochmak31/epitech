#ifndef RECIPEBOOK_HPP
#define RECIPEBOOK_HPP

#include <map>
#include <mutex>
#include <set>

#include "Ingredients.hpp"
#include "PizzaInfo.hpp"

class RecipeBook {
 public:
  static int getCookingTimeInMilliseconds(PizzaType pizzaType,
                                          PizzaSize pizzaSize);
  static std::set<Ingredients> getRecipe(
      PizzaType pizzaType);  // Thread-safe method to get a recipe
  static float CookingTimeMultiplier;

 private:
  static std::map<PizzaType, int> bakingTimes;
  static std::map<PizzaType, std::set<Ingredients>> Recipes;
  static std::mutex recipeMutex;
};

#endif  // RECIPEBOOK_HPP
