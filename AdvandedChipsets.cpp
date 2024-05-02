#include "Domain.cpp"
#include "ElementaryComponents.cpp"
#include <array>

#pragma once

namespace nts :: chipsets
{

    class Chipset4013 : public nts::MultiPinComponent, public nts::NamedComponent
    {
        friend class ComponentFactory;

        private:
            Chipset4013() {
                // Flip Flop 1
                auto ff1 = nts::elementary::DFlipFlop{};
                _links[3] = {&ff1, nts::elementary::DFlipFlop::ClockPin}; // CLK1
                _links[4] = {&ff1, nts::elementary::DFlipFlop::ClearPin}; // R1
                _links[5] = {&ff1, nts::elementary::DFlipFlop::DataPin};  // D1
                _links[6] = {&ff1, nts::elementary::DFlipFlop::SetPin};   // S1
                _links[1] = {&ff1, nts::elementary::DFlipFlop::OutputQPin}; // Q1
                _links[2] = {&ff1, nts::elementary::DFlipFlop::OutputNQPin}; // Q'1

                // Flip Flop 2
                auto ff2 = nts::elementary::DFlipFlop{};
                _links[11] = {&ff2, nts::elementary::DFlipFlop::ClockPin};  // CLK2
                _links[10] = {&ff2, nts::elementary::DFlipFlop::ClearPin};  // R2
                _links[9] = {&ff2, nts::elementary::DFlipFlop::DataPin};    // D2
                _links[8] = {&ff2, nts::elementary::DFlipFlop::SetPin};     // S2
                _links[13] = {&ff2, nts::elementary::DFlipFlop::OutputQPin};  // Q2
                _links[12] = {&ff2, nts::elementary::DFlipFlop::OutputNQPin}; // Q'2

                // Unused pins
                _links[7] = {&nts::DummyComponent::Instance(), 0};  // GND
                _links[14] = {&nts::DummyComponent::Instance(), 0}; // VDD
            }
    };

    class Chipset4008 : public nts::MultiPinComponent, public nts::NamedComponent
    {
        friend class ComponentFactory;

        private:
            Chipset4008() {
                // Manually linking each Full Adder's pins
                auto fa0 = nts::elementary::FullAdder{};
                _links[7] = {&fa0, nts::elementary::FullAdder::InputA};    // A0
                _links[6] = {&fa0, nts::elementary::FullAdder::InputB};    // B0
                _links[9] = {&fa0, nts::elementary::FullAdder::CarryIn};   // C_IN

                auto fa1 = nts::elementary::FullAdder{};
                _links[5] = {&fa1, nts::elementary::FullAdder::InputA};    // A1
                _links[4] = {&fa1, nts::elementary::FullAdder::InputB};    // B1
                // Carry link between FA0 and FA1...

                auto fa2 = nts::elementary::FullAdder{};
                _links[3] = {&fa2, nts::elementary::FullAdder::InputA};    // A2
                _links[2] = {&fa2, nts::elementary::FullAdder::InputB};    // B2
                // Carry link between FA1 and FA2...

                auto fa3 = nts::elementary::FullAdder{};
                _links[1] = {&fa3, nts::elementary::FullAdder::InputA};    // A3
                _links[15] = {&fa3, nts::elementary::FullAdder::InputB};   // B3
                // Carry link between FA2 and FA3...

                // Output Sums
                _links[10] = {&fa0, nts::elementary::FullAdder::SumOutput}; // S0
                _links[11] = {&fa1, nts::elementary::FullAdder::SumOutput}; // S1
                _links[12] = {&fa2, nts::elementary::FullAdder::SumOutput}; // S2
                _links[13] = {&fa3, nts::elementary::FullAdder::SumOutput}; // S3

                // Carry Output
                _links[14] = {&fa3, nts::elementary::FullAdder::CarryOut};  // C_OUT

                // Unused pins
                _links[8] = {&nts::DummyComponent::Instance(), 0};  // GND
                _links[16] = {&nts::DummyComponent::Instance(), 0}; // VDD
            }
    };

    class Chipset4017 : public nts::MultiPinComponent, public nts::NamedComponent
    {
    private:
        int _currentCount = 0;
        static constexpr int NUM_OUTPUTS = 10;
        std::array<nts::Tristate, NUM_OUTPUTS> _outputStates{};  // Stores the state of each output

    public:
        Chipset4017() {
            // Initialize all outputs to False initially
            _outputStates.fill(nts::Tristate::False);
            // Set the first output (Q0) to True initially
            _outputStates[0] = nts::Tristate::True;

            // Connect dummy components to unused pins
            _links[8] = {&nts::DummyComponent::Instance(), 0};  // GND
            _links[16] = {&nts::DummyComponent::Instance(), 0}; // VDD
        }

        nts::Tristate compute(std::size_t pin) override {
            if (pin >= 2 && pin <= 11) {
                // Outputs are Q0 to Q9 corresponding to pins 2 to 11
                return _outputStates[pin - 2];
            }
            throw InvalidOperationException("Invalid pin for 4017 compute.");
        }

        void clock(nts::Tristate clk) {
            if (clk == nts::Tristate::True) {  // Rising edge
                advanceCounter();
            }
        }

        void reset() {
            _currentCount = 0;
            updateOutputs();
        }

    private:
        void advanceCounter() {
            _currentCount = (_currentCount + 1) % NUM_OUTPUTS;
            updateOutputs();
        }

        void updateOutputs() {
            // Set all outputs to False
            _outputStates.fill(nts::Tristate::False);
            // Set the current output to True
            _outputStates[_currentCount] = nts::Tristate::True;
        }
    };


    class Chipset4040 : public nts::MultiPinComponent, public nts::NamedComponent
    {
        private:
            uint16_t _currentCount = 0;  // Enough to hold 12 bits
            static constexpr int NUM_BITS = 12;

            void incrementCounter() {
                _currentCount++;
            }

        public:
            Chipset4040() {
                // Connect outputs Q0 to Q11
                for (int i = 1; i <= 12; i++) {
                    _links[i] = {&nts::DummyComponent::Instance(), 0};  // Outputs initialized
                }

                // Unused pins are connected to dummy to avoid unhandled exceptions
                _links[8] = {&nts::DummyComponent::Instance(), 0};  // GND
                _links[16] = {&nts::DummyComponent::Instance(), 0}; // VDD
            }

            nts::Tristate compute(std::size_t pin) override {
                if (pin >= 1 && pin <= 12) {
                    return (_currentCount & (1 << (pin - 1))) ? nts::Tristate::True : nts::Tristate::False;
                }
                throw InvalidOperationException("Invalid pin for 4040 compute.");
            }

            void clock(Tristate clk) {
                if (clk == nts::Tristate::False) {  // Falling edge
                    incrementCounter();
                }
            }

            void reset() {
                _currentCount = 0;
            }
    };


}
