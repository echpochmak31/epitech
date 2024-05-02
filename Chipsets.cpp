#include "Domain.cpp"
#include "ElementaryComponents.cpp"

#pragma once

namespace nts :: chipsets
{

    class Chipset4001 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4001 ()
            {
                auto gate1 = nts :: elementary :: Or{};
                auto gate2 = nts :: elementary :: Or{};
                auto gate3 = nts :: elementary :: Or{};
                auto gate4 = nts :: elementary :: Or{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: FirstInputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: SecondInputPin}},
                    {3, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: FirstInputPin}},
                    {6, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: SecondInputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: OutputPin}},

                    {12, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: FirstInputPin}},
                    {13, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: SecondInputPin}},
                    {11, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: OutputPin}},

                    {8, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: FirstInputPin}},
                    {9, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: SecondInputPin}},
                    {10, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }

        public:
            nts :: Tristate compute (std :: size_t pin) override
            {
                auto result = MultiPinComponent :: compute(pin);
                
                return nts :: elementary :: Not :: evaluate(result);
            }

    };

    class Chipset4011 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4011 ()
            {
                auto gate1 = nts :: elementary :: And{};
                auto gate2 = nts :: elementary :: And{};
                auto gate3 = nts :: elementary :: And{};
                auto gate4 = nts :: elementary :: And{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: FirstInputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: SecondInputPin}},
                    {3, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: FirstInputPin}},
                    {6, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: SecondInputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: OutputPin}},

                    {12, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: FirstInputPin}},
                    {13, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: SecondInputPin}},
                    {11, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: OutputPin}},

                    {8, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: FirstInputPin}},
                    {9, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: SecondInputPin}},
                    {10, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }

        public:
            nts :: Tristate compute (std :: size_t pin) override
            {
                auto result = MultiPinComponent :: compute(pin);
                
                return nts :: elementary :: Not :: evaluate(result);
            }
    };

    
    class Chipset4030 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4030 ()
            {
                auto gate1 = nts :: elementary :: Xor{};
                auto gate2 = nts :: elementary :: Xor{};
                auto gate3 = nts :: elementary :: Xor{};
                auto gate4 = nts :: elementary :: Xor{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: FirstInputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: SecondInputPin}},
                    {3, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: FirstInputPin}},
                    {6, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: SecondInputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: OutputPin}},

                    {12, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: FirstInputPin}},
                    {13, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: SecondInputPin}},
                    {11, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: OutputPin}},

                    {8, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: FirstInputPin}},
                    {9, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: SecondInputPin}},
                    {10, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }
    };
    

    class Chipset4069 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4069 ()
            {
                auto gate1 = nts :: elementary :: Not{};
                auto gate2 = nts :: elementary :: Not{};
                auto gate3 = nts :: elementary :: Not{};
                auto gate4 = nts :: elementary :: Not{};
                auto gate5 = nts :: elementary :: Not{};
                auto gate6 = nts :: elementary :: Not{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: InOutComponent :: InputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: InOutComponent :: OutputPin}},

                    {3, nts :: ComponentDetails {&gate2, nts :: InOutComponent :: InputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: InOutComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate3, nts :: InOutComponent :: InputPin}},
                    {6, nts :: ComponentDetails {&gate3, nts :: InOutComponent :: OutputPin}},

                    {9, nts :: ComponentDetails {&gate4, nts :: InOutComponent :: InputPin}},
                    {8, nts :: ComponentDetails {&gate4, nts :: InOutComponent :: OutputPin}},

                    {11, nts :: ComponentDetails {&gate5, nts :: InOutComponent :: InputPin}},
                    {10, nts :: ComponentDetails {&gate5, nts :: InOutComponent :: OutputPin}},

                    {13, nts :: ComponentDetails {&gate6, nts :: InOutComponent :: InputPin}},
                    {12, nts :: ComponentDetails {&gate6, nts :: InOutComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }
    };

    class Chipset4071 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4071 ()
            {
                auto gate1 = nts :: elementary :: Or{};
                auto gate2 = nts :: elementary :: Or{};
                auto gate3 = nts :: elementary :: Or{};
                auto gate4 = nts :: elementary :: Or{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: FirstInputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: SecondInputPin}},
                    {3, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: FirstInputPin}},
                    {6, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: SecondInputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: OutputPin}},

                    {12, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: FirstInputPin}},
                    {13, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: SecondInputPin}},
                    {11, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: OutputPin}},

                    {8, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: FirstInputPin}},
                    {9, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: SecondInputPin}},
                    {10, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }
    };


    class Chipset4081 : public nts :: MultiPinComponent, public nts :: NamedComponent
    {
        friend class ComponentFactory;

        public:
            Chipset4081 ()
            {
                auto gate1 = nts :: elementary :: And{};
                auto gate2 = nts :: elementary :: And{};
                auto gate3 = nts :: elementary :: And{};
                auto gate4 = nts :: elementary :: And{};
                
                _links = {
                    {1, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: FirstInputPin}},
                    {2, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: SecondInputPin}},
                    {3, nts :: ComponentDetails {&gate1, nts :: ThreePinComponent :: OutputPin}},

                    {5, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: FirstInputPin}},
                    {6, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: SecondInputPin}},
                    {4, nts :: ComponentDetails {&gate2, nts :: ThreePinComponent :: OutputPin}},

                    {12, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: FirstInputPin}},
                    {13, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: SecondInputPin}},
                    {11, nts :: ComponentDetails {&gate3, nts :: ThreePinComponent :: OutputPin}},

                    {8, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: FirstInputPin}},
                    {9, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: SecondInputPin}},
                    {10, nts :: ComponentDetails {&gate4, nts :: ThreePinComponent :: OutputPin}},

                    {7, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                    {14, nts :: ComponentDetails {&nts :: DummyComponent :: Instance(), 0}},
                };
            }
    };

}
