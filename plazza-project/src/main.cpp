#include <iostream>
#include <unistd.h> // for fork, pipe, close
#include <sys/types.h> // for pid_t
#include <sys/wait.h> // for wait
#include "Reception.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <cooking_time_multiplier> <num_cooks> <replenish_time>" << std::endl;
        return 1;
    }

    float cookingTimeMultiplier = std::stof(argv[1]);
    int numCooks = std::stoi(argv[2]);
    int replenishTime = std::stoi(argv[3]);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) { // Child process
        close(pipefd[0]); // Close the read end
        std::string command;
        while (true) {
            std::cout << "Enter command: ";
            std::getline(std::cin, command);
            command += '\n'; // Add newline to match the delimiter used in parent process
            if (write(pipefd[1], command.c_str(), command.size()) == -1) {
                perror("write");
                break;
            }
            if (command == "exit\n") {
                break;
            }
        }
        close(pipefd[1]); // Close the write end
    } else { // Parent process
        close(pipefd[1]); // Close the write end
        Reception reception(numCooks, replenishTime, cookingTimeMultiplier);
        reception.handleCommands(pipefd[0]);
        close(pipefd[0]); // Close the read end
        wait(nullptr); // Wait for the child process to finish
    }

    return 0;
}
