#ifndef KITCHENDETAILS_HPP
#define KITCHENDETAILS_HPP
#include <sched.h>
#include <string>

class KitchenDetails {
public:
    pid_t Pid;
    std::string IpcAddress;
};

#endif //KITCHENDETAILS_HPP
