#include "Chipsets.cpp"
#include "SpecialComponents.cpp"

#include <memory>
#include <unordered_map>
#include <functional>

#pragma once

namespace nts {
    
    class ComponentFactory
    {
        public:
            std :: unique_ptr<IComponent> create(const std :: string& key) {
                auto it = factoryMap.find(key);
                if (it != factoryMap.end())
                {
                    return it->second();
                }
                else
                {
                    auto message = "Unknown component type: " + key;
                    throw nts :: UnknownComponentTypeException(message.c_str());
                }
            }

        private:
            const static std :: unordered_map<std :: string, std :: function<std :: unique_ptr<IComponent>()>> factoryMap;
    };

    const std :: unordered_map<std :: string, std :: function<std :: unique_ptr<IComponent>()>> ComponentFactory :: factoryMap = {

        {"input", []() { return std::make_unique<nts :: special :: Input>(); }},
        {"output", []() { return std::make_unique<nts :: special :: Output>(); }},

        {"4001", []() { return std::make_unique<nts :: chipsets :: Chipset4001>(); }},
        {"4011", []() { return std::make_unique<nts :: chipsets :: Chipset4011>(); }},
        {"4030", []() { return std::make_unique<nts :: chipsets :: Chipset4030>(); }},
        {"4069", []() { return std::make_unique<nts :: chipsets :: Chipset4069>(); }},
        {"4071", []() { return std::make_unique<nts :: chipsets :: Chipset4071>(); }},
        {"4081", []() { return std::make_unique<nts :: chipsets :: Chipset4081>(); }},
    };

}