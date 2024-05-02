#include "Exceptions.cpp"
#include <iostream>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#pragma once

namespace nts
{

    enum Tristate
    {
        Undefined = (- true ),
        True = true ,
        False = false
    };

    class IComponent
    {
        public:
            virtual ~ IComponent () = default ;

            virtual nts :: Tristate compute (std :: size_t pin) = 0;

            virtual void setLink (std :: size_t pin, nts :: IComponent &other, std :: size_t otherPin) = 0;

    };

    // typedef std :: unique_ptr<IComponent> IComponentPtr;

    class IMutableComponent
    {
        public:
            virtual void setValue (nts :: Tristate value) = 0;
    };


    class NamedComponent
    {
        protected:
            std :: string _name = "";

        public:
            std :: string getName()
            {
                return _name;
            }

            void setName(std :: string name)
            {
                _name = name;
            }
    };


    struct ComponentDetails
    {
        nts :: IComponent* Component;
        std :: size_t Pin;
    };


    class SinglePinComponent : public IComponent
    {
        protected:
            SinglePinComponent() = default;

            ComponentDetails _link;

        public:
            static const std :: size_t Pin = 1;

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override {
                _link = nts :: ComponentDetails {&other, otherPin};
            }

    };


    class MultiPinComponent : public IComponent
    {
        protected:
            MultiPinComponent() = default;

            std :: unordered_map<std :: size_t, nts :: ComponentDetails> _links;
        public:
            virtual nts :: Tristate compute (std :: size_t pin) override
            {
                auto link = _links[pin];

                std :: cout << "yep" << std :: endl; 
                std :: cout << pin << std :: endl; 

                return link.Component->compute(link.Pin);
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override 
            {
                auto componentDetails = ComponentDetails { &other, otherPin };
                _links[pin] = componentDetails;
            }
    };


    class ThreePinComponent : public MultiPinComponent
    {
        public:
            virtual nts::Tristate compute(nts::Tristate stateA, nts::Tristate stateB) = 0;

            static const std :: size_t FirstInputPin = 1;

            static const std :: size_t SecondInputPin = 2;

            static const std :: size_t OutputPin = 3;
    };


    class InOutComponent : public MultiPinComponent
    {
        protected:
            InOutComponent() = default;
            ComponentDetails _link;
        public:
            static const std :: size_t InputPin = 1;
            static const std :: size_t OutputPin = 2;
    };

    class DummyComponent : public IComponent
    {
        private:
            DummyComponent() { }
            DummyComponent(const DummyComponent&) = delete;
            DummyComponent& operator=(const DummyComponent&) = delete;

        public:
            nts :: Tristate compute (std :: size_t pin) override
            {
                throw nts :: InvalidOperationException("Dummy component's `compute` method must not be invoked.");
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override
            {
                throw nts :: InvalidOperationException("Dummy component's `setLink` method must not be invoked.");
            }

            static DummyComponent& Instance() {
                static DummyComponent instance;

                return instance;
            }
    };
    
}
