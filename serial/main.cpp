#include "SerialConnection.h"
#include "CommandHandler.h"
#include "SignalHandler.h"
#include <iostream>

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return 1;
    }

    const std::string port = argv[1];

    try {
        // SerialConnection serial(port, 115200);

        // Note that we need a pointer in order to implement the signal handler for cleanup.
        SerialConnection* serial = new SerialConnection(port, 115200);
        registerHandler(serial);


        CommandHandler handler(*serial);

        std::string userCommand;
        while (true) {
            std::cout << "Enter command: ";
            std::getline(std::cin, userCommand);

            if (userCommand == "exit") {
                break;
            }

            handler.processUserCommand(userCommand);
        }
        // Optional cleanup (not needed since the signal handler will take care of it)
        delete serial;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
