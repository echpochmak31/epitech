#include "IngredientStock.hpp"

#include <chrono>
#include <sstream>
#include <thread>

#include "Constants.hpp"
#include "RecipeBook.hpp"

IngredientStock::IngredientStock(int replenishTimeInMilliseconds)
    : replenishTimeInMilliseconds(replenishTimeInMilliseconds), running(true) {
  for (int i = 0; i < static_cast<int>(Ingredients::COUNT); i++) {
    auto ingredient = static_cast<Ingredients>(i);
    stock[ingredient] = INITIAL_INGREDIENT_COUNT;
  }
  replenishThread = std::thread(&IngredientStock::replenish, this);
}

IngredientStock::~IngredientStock() {
  running = false;
  if (replenishThread.joinable()) {
    replenishThread.join();
  }
}

void IngredientStock::replenish() {
  while (running) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(replenishTimeInMilliseconds));
    replenishStock();
  }
}

void IngredientStock::replenishStock() {
  std::lock_guard<std::mutex> lock(stockMutex);
  for (auto& item : stock) {
    item.second++;
  }
}

bool IngredientStock::useIngredients(const std::set<Ingredients>& ingredients) {
  std::lock_guard<std::mutex> lock(stockMutex);
  for (const auto& ingredient : ingredients) {
    if (stock[ingredient] <= 0) {
      return false;  // Not enough stock
    }
  }
  for (const auto& ingredient : ingredients) {
    stock[ingredient]--;
  }
  return true;
}

std::string IngredientStock::getStatus() {
  std::lock_guard<std::mutex> lock(stockMutex);
  std::ostringstream status;
  for (const auto& item : stock) {
    status << static_cast<int>(item.first) << " - " << item.second << "; ";
  }
  return std::move(status).str();
  ;
}
