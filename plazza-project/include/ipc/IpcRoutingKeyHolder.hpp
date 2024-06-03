#ifndef IPCROUTINGKEYHOLDER_HPP
#define IPCROUTINGKEYHOLDER_HPP
#include <string>

class IpcRoutingKeyHolder {
public:
    static const char* CloseKitchen;
    static const char* GetKitchenStatus;
    static const char* AcceptOrderedPizza;
    static const char* OrderedPizzaReady;
    static const char* AllCooksAreBusy;
};

#endif //IPCROUTINGKEYHOLDER_HPP
