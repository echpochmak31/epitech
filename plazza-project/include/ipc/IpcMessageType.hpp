#ifndef MESSAGETYPE_HPP
#define MESSAGETYPE_HPP

#include <string>

enum class IpcMessageType : uint8_t {
    UNKNOWN = 0,
    SIGNAL = 1, // one-way interaction
    EVENT = 2, // signal with many recipients
    REQUEST = 3, // two-way interaction
    RESPONSE = 4, // two-way interaction
};

#endif //MESSAGETYPE_HPP
