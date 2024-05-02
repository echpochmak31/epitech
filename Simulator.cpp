#include "Domain.cpp"
#include "Factory.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <csignal>

#pragma once

class CircuitSimulator {
public:
    CircuitSimulator(const std::string &filename) {
        if (!parseFile(filename)) {
            std::cerr << "Error parsing file: " << filename << std::endl;
            exit(84);  // Indicate error
        }
        currentTick = 0;
    }

    void run() {
        try
        {
            std::string command;
            while (true) {
                
                std::cout << "> ";
                if (!getline(std::cin, command)) {
                    break;  // Exit on EOF (CTRL+D)
                }

                try
                {
                    if (command == "exit") {
                        break;
                    } else if (command == "display") {
                        display();
                    } else if (command == "simulate") {
                        simulate();
                    } else if (command == "loop") {
                        runLoop();
                    } else if (command.find('=') != std::string::npos) {
                        setInputValue(command);
                    } else {
                        std::cout << "Unknown command" << std::endl;
                    }
                }
                catch(const nts::NanoTekSpiceException& e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << "Fatal error!\n";
            std::cerr << e.what() << '\n';
            exit(84);
        }

    }

private:
    nts :: ComponentFactory factory;
    std::unordered_map<std::string,  std::unique_ptr<nts :: IComponent>> components;
    std::map<std::string, nts::IMutableComponent*> inputs;
    std::map<std::string, std::size_t> outputs;
    std::map<std::string, nts::Tristate> outputResults;
    int currentTick;

    bool parseFile(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Unable to open file" << std::endl;
            return false;
        }

        std::string line;
        bool inChipsets = false;
        bool inLinks = false;

        while (getline(file, line)) {
            if (line == ".chipsets:") {
                inChipsets = true;
                inLinks = false;
            } else if (line == ".links:") {
                inChipsets = false;
                inLinks = true;
            } else if (inChipsets) {
                parseChipsets(line);
            } else if (inLinks) {
                parseLinks(line);
            }
        }

        file.close();
        return true;
    }

    void parseChipsets(const std::string &line) {
        std::stringstream ss(line);
        std::string componentType;
        std::string componentName;

        ss >> componentType >> componentName;

        auto component = factory.create(componentType);

        auto it = components.find(componentName);
        if (it != components.end())
        {
            auto message = "Components with name `" + componentName + "` is already definied.";
            throw nts :: NonUniqueComponentNameException(message.c_str());
        }
        else
        {
            components[componentName] = std::move(component);
        }

        if (componentType == "output") {
            outputs[componentName] = nts::SinglePinComponent::Pin;
        }

    }

    void parseLinks(const std::string &line) {
        std::stringstream ss(line);
        std::string source, destination;

        std::getline(ss, source, ' ');
        std::getline(ss, destination);
        
        std::string sourceName;
        int sourcePin = -1;

        std::string destinationName;
        int destinationPin = -1;

        if (source.find(':') != std::string::npos) {
            sourceName = source.substr(0, source.find(':'));
            std::string sourcePinStr = source.substr(source.find(':') + 1);

            try {
                sourcePin = std::stoi(sourcePinStr);
            } catch (const std::invalid_argument &e) {
                auto message = "Error parsing source pin: " + source;
                throw nts::InvalidSyntaxException(message.c_str());
            }
        }

        if (destination.find(':') != std::string::npos) {
            destinationName = destination.substr(0, destination.find(':'));
            std::string destinationPinStr = destination.substr(destination.find(':') + 1);

            try {
                destinationPin = std::stoi(destinationPinStr);
            } catch (const std::invalid_argument &e) {
                auto message = "Error parsing destination pin: " + destination;
                throw nts::InvalidSyntaxException(message.c_str());
            }
        }

        if (sourcePin == -1 || destinationPin == -1) {
            auto message = "Error parsing link: " + source + " -> " + destination;
            throw nts::InvalidSyntaxException(message.c_str());
        }

        // todo: fix linking
        components[sourceName]->setLink(sourcePin, *components[destinationName], destinationPin);
        components[destinationName]->setLink(destinationPin, *components[sourceName], sourcePin);
    }


    void display() {
        std::cout << "tick: " << currentTick << std::endl;
        std::cout << "input(s):" << std::endl;
        for (auto &input : inputs) {
            auto result = components[input.first].get()->compute(nts::SinglePinComponent::Pin);
            std::cout << input.first << ": " << valueTostring(result) << std::endl;
        }
        std::cout << "output(s):" << std::endl;
        for (auto &output : outputs) {
            auto result = outputResults[output.first];
            std::cout << output.first << ": " << valueTostring(result) << std::endl;
        }
    }

    void simulate() {
        currentTick++;

        for (auto &output : outputs) {
            auto result = components[output.first].get()->compute(output.second);
            outputResults[output.first] = result;
        }
    }

    void setInputValue(const std::string &command) {
        size_t equalPos = command.find('=');
        std::string inputName = command.substr(0, equalPos);
        std::string value = command.substr(equalPos + 1);

        nts :: Tristate logicValue;
        if (value == "0") {
            logicValue = nts :: Tristate :: False;
        } else if (value == "1") {
            logicValue = nts :: Tristate :: True;
        } else {
            logicValue = nts :: Tristate :: Undefined;
        }

        if (auto mutableComponentPtr = dynamic_cast<nts::IMutableComponent*>(components[inputName].get())) {
            inputs[inputName] = mutableComponentPtr;
            inputs[inputName]->setValue(logicValue);
        } else {
            throw nts::InvalidSyntaxException("Only components of type `input` or `clock` are mutable.");
        }

    }

    void runLoop() {
        signal(SIGINT, handleSignalInterrupt);
        while (true) {
            simulate();
            display();
            if (receivedInterrupt) {
                receivedInterrupt = false;
                break;
            }
        }
    }

    static void handleSignalInterrupt(int signal) {
        if (signal == SIGINT) {
            receivedInterrupt = true;
        }
    }

    static bool receivedInterrupt;

    static std::string valueTostring(nts :: Tristate value) {
        if (value == nts :: Tristate :: False) {
            return "0";
        } else if (value == nts :: Tristate :: True) {
            return "1";
        } else {
            return "U";
        }
    }
};

bool CircuitSimulator::receivedInterrupt = false;