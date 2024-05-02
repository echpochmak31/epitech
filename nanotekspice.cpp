#include "Simulator.cpp"

#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return 1;
    }

    CircuitSimulator simulator(argv[1]);
    simulator.run();

    return 0;
}


