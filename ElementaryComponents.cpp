#include "Domain.cpp"

#pragma once

namespace nts :: elementary
{

    struct And : public nts :: ThreePinComponent 
    {
        virtual nts::Tristate compute(nts::Tristate stateA, nts::Tristate stateB) override
        {
            return evaluate(stateA, stateB);
        }

        static nts::Tristate evaluate(nts::Tristate stateA, nts::Tristate stateB)
        {
            if (stateA == nts::Tristate::True && stateB == nts::Tristate::True) {
                return nts::Tristate::True;
            }

            if (stateA == nts::Tristate::False || stateB == nts::Tristate::False) {
                return nts::Tristate::False;
            }

            return nts::Tristate::Undefined;
        }
    };

    struct Or : public nts :: ThreePinComponent 
    {
        virtual nts::Tristate compute(nts::Tristate stateA, nts::Tristate stateB) override
        {
            return evaluate(stateA, stateB);
        }

        nts::Tristate compute(std::size_t pin) override
        {
            auto componentA = _links[1];
            auto inputA = componentA.Component->compute(componentA.Pin);

            auto componentB = _links[2];
            auto inputB = componentB.Component->compute(componentB.Pin);

            return evaluate(inputA, inputB);
        }

        static nts::Tristate evaluate(nts::Tristate stateA, nts::Tristate stateB)
        {
            if (stateA == nts::Tristate::True || stateB == nts::Tristate::True) {
                return nts::Tristate::True;
            }

            if (stateA == nts::Tristate::False && stateB == nts::Tristate::False) {
                return nts::Tristate::False;
            }

            return nts::Tristate::True;
        }
    };

    
    struct Xor : public nts :: ThreePinComponent 
    {
        virtual nts::Tristate compute(nts::Tristate stateA, nts::Tristate stateB) override
        {
            return evaluate(stateA, stateB);
        }

        static nts::Tristate evaluate(nts::Tristate stateA, nts::Tristate stateB)
        {
            if (stateA == nts::Tristate::Undefined || stateB == nts::Tristate::Undefined) {
                return nts::Tristate::Undefined;
            }

            if (stateA == stateB) {
                return nts::Tristate::False;
            }

            return nts::Tristate::True;
        }
    };
    

    struct Not : public nts :: InOutComponent 
    {
        nts :: Tristate compute (std :: size_t pin) override {
            return evaluate(_link.Component->compute(_link.Pin));
        }

        void setLink (std :: size_t pin , nts :: IComponent &other , std :: size_t otherPin) {
            _link = nts :: ComponentDetails {&other, otherPin};
        }

        static nts::Tristate evaluate(nts::Tristate state) {
            if (state == nts::Tristate::False) {
                return nts::Tristate::True;
            }

            if (state == nts::Tristate::True) {
                return nts::Tristate::False;
            }

            return nts::Tristate::Undefined;
        }
    };


    class DFlipFlop : public SinglePinComponent {
    private:
        Tristate _currentState = Undefined;
        Tristate _nextState = Undefined;

    public:
        void clock(Tristate clockSignal)
        {
            if (clockSignal == True && _link.Component)
            {
                _nextState = _link.Component->compute(_link.Pin);
            }
            else if (clockSignal == False && _currentState != _nextState)
            {
                _currentState = _nextState;
            }
        }

        Tristate compute(std::size_t pin) override {
            if (pin == Pin)
            {
                return _currentState;
            }

            return Undefined;  // or throw an exception
        }
    };


    class FullAdder : public MultiPinComponent
    {
        private:
            std::pair<Tristate, Tristate> calculateFullAdder(Tristate a, Tristate b, Tristate carryIn)
            {
                auto sum = Xor::evaluate(Xor::evaluate(a, b), carryIn);
                auto carryOut = Or::evaluate(And::evaluate(a, b), And::evaluate(Xor::evaluate(a, b), carryIn));
                return {sum, carryOut};
            }

        public:
            static const std::size_t InputA = 1;
            static const std::size_t InputB = 2;
            static const std::size_t CarryIn = 3;
            static const std::size_t SumOutput = 4;
            static const std::size_t CarryOut = 5;
            Tristate compute(std::size_t pin) override
            {
                if (_links.find(InputA) == _links.end() || _links.find(InputB) == _links.end() || _links.find(CarryIn) == _links.end())
                    throw InvalidOperationException("FullAdder: Not all input pins are connected.");

                auto a = _links[InputA].Component->compute(_links[InputA].Pin);
                auto b = _links[InputB].Component->compute(_links[InputB].Pin);
                auto carry = _links[CarryIn].Component->compute(_links[CarryIn].Pin);

                auto result = calculateFullAdder(a, b, carry);

                if (pin == SumOutput)
                    return result.first;
                if (pin == CarryOut)
                    return result.second;

                throw InvalidOperationException("FullAdder: Invalid pin requested.");
            }

            void setLink(std::size_t pin, IComponent &other, std::size_t otherPin) override
            {
                if (pin < InputA || pin > CarryOut)
                    throw InvalidOperationException("FullAdder: Invalid pin number.");

                _links[pin] = { &other, otherPin };
            }
    };
}
