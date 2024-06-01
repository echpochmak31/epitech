#include "ipc/IpcMessageType.hpp"

const std::string IpcMessageType::valueToString[] = {
    "UNKNOWN",
    "CLOSE_KITCHEN_SIGNAL",
    "GET_KITCHEN_STATUS_REQUEST",
    "GET_KITCHEN_STATUS_RESPONSE",
};

const std::map<std::string, IpcMessageType::Value> IpcMessageType::stringToValue = {
    {"UNKNOWN", IpcMessageType::UNKNOWN},
    {"CLOSE_KITCHEN_SIGNAL", IpcMessageType::CLOSE_KITCHEN_SIGNAL},
    {"GET_KITCHEN_STATUS_REQUEST", IpcMessageType::GET_KITCHEN_STATUS_REQUEST},
    {"GET_KITCHEN_STATUS_RESPONSE", IpcMessageType::GET_KITCHEN_STATUS_RESPONSE},
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
