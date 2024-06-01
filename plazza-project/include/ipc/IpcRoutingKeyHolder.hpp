#ifndef IPCROUTINGKEYHOLDER_HPP
#define IPCROUTINGKEYHOLDER_HPP
#include <string>

class IpcRoutingKeyHolder {
public:
    static const char* GetKitchenStatusRequest;
    static const char* GetKitchenStatusResponse;
    static const char* CloseKitchen;
};

#endif //IPCROUTINGKEYHOLDER_HPP
