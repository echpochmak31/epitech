#ifndef MESSAGETYPE_HPP
#define MESSAGETYPE_HPP

#include <map>
#include <string>

class IpcMessageType {
public:
    enum Value : uint8_t {
        UNKNOWN,
        COUNT_BUSY_COOKS_REQUEST,
        COUNT_BUSY_COOKS_RESPONSE,
        CLOSE_KITCHEN,
        GET_KITCHEN_STATUS,
        KITCHEN_AVAILABLE,
        KITCHEN_UNAVAILABLE,
    };

    IpcMessageType(Value value);
    ~IpcMessageType() = default;

    IpcMessageType::Value getValue() const;
    std::string toString() const;
    static std::string toString(IpcMessageType::Value value);
    static IpcMessageType::Value fromString(const std::string& value);
private:
    Value value;
    static const std::string valueToString[];
    static const std::map<std::string, IpcMessageType::Value> stringToValue;
};

#endif //MESSAGETYPE_HPP
