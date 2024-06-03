#ifndef UTILS_HPP
#define UTILS_HPP
#include <chrono>
#include <iomanip>
#include <sstream>

std::string timePointToString(const std::chrono::system_clock::time_point &tp);

#endif //UTILS_HPP
