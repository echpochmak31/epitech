#ifndef PLAZZA_PROJECT_COOK_HPP
#define PLAZZA_PROJECT_COOK_HPP

#include <atomic>
#include <thread>
#include <chrono>

class Cook {
private:
    std::thread cookThread;
    std::atomic<bool> _isBusy;

public:
    Cook() : _isBusy(false) {}

    // Disable copy constructor and copy assignment operator
    Cook(const Cook&) = delete;
    Cook& operator=(const Cook&) = delete;

    // Enable move constructor and move assignment operator
    Cook(Cook&& other) noexcept : cookThread(std::move(other.cookThread)), _isBusy(other._isBusy.load()) {
        other._isBusy = false;
    }
    Cook& operator=(Cook&& other) noexcept {
        if (this != &other) {
            if (cookThread.joinable()) {
                cookThread.join();
            }
            cookThread = std::move(other.cookThread);
            _isBusy = other._isBusy.load();
            other._isBusy = false;
        }
        return *this;
    }

    void startCooking() {
        _isBusy = true;
        cookThread = std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Mock cooking time
            _isBusy = false;
        });
        cookThread.detach();
    }

    bool isBusy() const {
        return _isBusy;
    }
};

#endif // PLAZZA_PROJECT_COOK_HPP
