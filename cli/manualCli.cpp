#include <iostream>
#include <string>
#include "CommandLineInterface.h"

int main()
{
    CommandLineInterface cli;
    std::cout << "Simple CLI (type 'exit' to quit)" << std::endl;

    while (true) {
        std::cout << "> ";
        std::cout.flush();
        std::string command = cli.getCommand();

        if (command == "exit") {
            break;
        }
        // Process the command here
        std::cout << "You entered: " << command << std::endl;
    }

    return 0;
}