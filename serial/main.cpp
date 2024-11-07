#include "SerialConnection.h"
#include "CommandHandler.h"
#include "SignalHandler.h"
#include "Logger.h"
#include <iostream>
#include <string>

void printHelp() 
{
    std::cout << "Available commands:\n"
              << "  set <register_name> <value>    - Set register value\n"
              << "  get <register_name>            - Get register value\n"
              << "  exec <command_name>            - Execute command\n"
              << "  ramp <speed> <duration>        - Execute speed ramp\n"
              << "  log-start                      - Start logging\n"
              << "  log-stop                       - Stop logging\n"
              << "  log-add <register_name>        - Add register to logging\n"
              << "  log-remove <register_name>     - Remove register from logging\n"
              << "  log-status                     - Show logging status\n"
              << "  help                           - Show this help\n"
              << "  exit                           - Exit program\n";
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return 1;
    }

    try {
        SerialConnection serial(argv[1], 115200);
        
        // Configure logger
        Logger::LogConfig logConfig{
            .filename = "log.csv",
            .sampleInterval = std::chrono::milliseconds(100),
            .bufferSize = 1024,
            .useTimestamp = true
        };
        
        Logger logger(serial, logConfig);
        CommandHandler handler(serial, logger);
        
        // Setup signal handler
        SignalHandler::setup();
        
        std::cout << "Motor Control Interface\n"
                  << "Type 'help' for available commands\n";

        std::string userInput;
        while (!SignalHandler::shouldExit()) {
            std::cout << "> ";
            if (!std::getline(std::cin, userInput)) {
                break;  // Handle EOF
            }

            if (userInput == "exit") {
                break;
            }

            if (userInput == "help") {
                printHelp();
                continue;
            }

            if (!userInput.empty()) {
                auto result = handler.processCommand(userInput);
                if (result.success) {
                    std::cout << result.message << std::endl;
                } else {
                    std::cerr << "Error: " << result.message << std::endl;
                }
            }
        }

        // Cleanup
        if (logger.isRunning()) {
            std::cout << "Stopping logger..." << std::endl;
            logger.stop();
        }

        std::cout << "Exiting..." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}