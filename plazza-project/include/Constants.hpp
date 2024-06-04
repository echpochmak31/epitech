#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

constexpr auto ENABLE_DEBUG_LOGGING = false;

constexpr const char * DUMMY_PAYLOAD = ":";
constexpr int MAX_ORDERED_PIZZAS_MULTIPLIER = 2;
constexpr int INITIAL_INGREDIENT_COUNT = 5;
constexpr int KITCHEN_MONITORING_DEALY_IN_MILLISECONDS = 100;
constexpr int KITCHEN_IDLE_TIME_LIMIT_IN_SECONDS = 5;
constexpr const char * RECEPTION_DEFAULT_IPC_ADDRESS = "reception";
constexpr int RECEPTION_INITIAL_POOL_SIZE = 2;
constexpr int POLLING_TIMEOUT_IN_SECONDS = 2;

#endif //CONSTANTS_HPP
