#include "ipc/IpcMessageType.hpp"

const std::string IpcMessageType::valueToString[] = {
    "UNKNOWN",
    "COUNT_BUSY_COOKS_REQUEST",
    "COUNT_BUSY_COOKS_RESPONSE",
    "CLOSE_KITCHEN",
    "GET_KITCHEN_STATUS",
    "KITCHEN_AVAILABLE",
    "KITCHEN_UNAVAILABLE",
};

const std::map<std::string, IpcMessageType::Value> IpcMessageType::stringToValue = {
    {"UNKNOWN", IpcMessageType::UNKNOWN},
    {"COUNT_BUSY_COOKS_REQUEST", IpcMessageType::COUNT_BUSY_COOKS_REQUEST},
    {"COUNT_BUSY_COOKS_RESPONSE", IpcMessageType::COUNT_BUSY_COOKS_RESPONSE},
    {"CLOSE_KITCHEN", IpcMessageType::CLOSE_KITCHEN},
    {"GET_KITCHEN_STATUS", IpcMessageType::GET_KITCHEN_STATUS},
    {"KITCHEN_AVAILABLE", IpcMessageType::KITCHEN_AVAILABLE},
    {"KITCHEN_UNAVAILABLE", IpcMessageType::KITCHEN_UNAVAILABLE},
};

IpcMessageType::IpcMessageType(Value value) : value(value) {
}

IpcMessageType::Value IpcMessageType::getValue() const {
    return value;
}

std::string IpcMessageType::toString(IpcMessageType::Value value) {
    return valueToString[value];
}

IpcMessageType::Value IpcMessageType::fromString(const std::string &value) {
    const auto it = stringToValue.find(value);
    if (it != stringToValue.end()) {
        return it->second;
    }
    return IpcMessageType::UNKNOWN;
}

std::string IpcMessageType::toString() const {
    return valueToString[getValue()];
}
