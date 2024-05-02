#include "Domain.cpp"
#include "ElementaryComponents.cpp"

#pragma once

namespace nts :: special
{

    class Input : public nts :: IMutableComponent, public nts :: SinglePinComponent, public nts ::  NamedComponent
    {

        nts :: Tristate _value = nts :: Tristate :: Undefined;

        public:
            Input (std :: string name) {
                _name = name;
            }

            Input () {
            }


            nts :: Tristate compute (std :: size_t pin) override {
                return _value;
            }


            void setValue (nts :: Tristate value) {
                _value = value;
            }
    };


    class Clock : public nts :: IMutableComponent, public nts :: SinglePinComponent, public nts ::  NamedComponent
    {

        nts :: Tristate _value = nts :: Tristate :: Undefined;

        public:
            Clock (std :: string name) {
                _name = name;
            }

            nts :: Tristate compute (std :: size_t pin) {
                return nts :: elementary :: Not :: evaluate(_value);
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override {
                _link = nts :: ComponentDetails {&other, otherPin};
            }

            void setValue (nts :: Tristate value) {
                _value = value;
            }

    };

    
    class True : public nts :: IComponent, public nts :: SinglePinComponent, public nts :: NamedComponent
    {
        public:
            True (std :: string name) {
                _name = name;
            }

            nts :: Tristate compute (std :: size_t pin) override {

                return nts :: Tristate :: True;
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override {
                _link = nts :: ComponentDetails {&other, otherPin};
            }

    };
    

    class False : public nts :: IComponent, public nts :: SinglePinComponent, public nts ::  NamedComponent
    {
        public:
            False (std :: string name) {
                _name = name;
            }

            nts :: Tristate compute (std :: size_t pin) override {
                return nts :: Tristate :: False;
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override {
                _link = nts :: ComponentDetails {&other, otherPin};
            }
    };

    
    class Output : public nts :: SinglePinComponent, public nts ::  NamedComponent 
    {
        public:
            Output (std :: string name) {
                _name = name;
            }

            Output () {
            }

            nts :: Tristate compute (std :: size_t pin) override {
                return _link.Component->compute(_link.Pin);
            }

            void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin ) override {
                _link = nts :: ComponentDetails {&other, otherPin};
            }

    };

}
