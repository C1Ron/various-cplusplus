#include "SerialConnection.h"
#include "CommandHandler.h"
#include <iostream>

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return 1;
    }

    const std::string port = argv[1];

    try {
        // Open the serial port passed as argument
        SerialConnection serial(port, 115200);
        CommandHandler handler(serial);

        std::string userCommand;
        while (true) {
            std::cout << "Enter command: ";
            std::getline(std::cin, userCommand);

            if (userCommand == "exit") {
                break;
            }

            handler.processUserCommand(userCommand);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
