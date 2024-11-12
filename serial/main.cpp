#include "SerialConnection.h"
#include "CommandHandler.h"
#include "SignalHandler.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

void printHelp() 
{
    std::cout << "MPC commands:\n"
              << "\tset <register> <value>         - Set register value\n"
              << "\tget <register>                 - Get register value\n"
              << "\texec <command>                 - Execute command\n"
              << "\tramp <speed> <duration>        - Execute speed ramp\n"
              << "\tcurrent <Iq> <Id>              - Set current references\n"
              << "Logging commands:\n"
              << "\tlog-start                      - Start logging\n"
              << "\tlog-stop                       - Stop logging\n"
              << "\tlog-add <register>             - Add register to logging\n"
              << "\tlog-remove <register>          - Remove register from logging\n"
              << "\tlog-status                     - Show logging status\n"
              << "Other commands:\n"
              << "\thelp                           - Show this help\n"
              << "\thelp-reg                       - Show all available registers and associated types\n"
              << "\thelp-exec                      - Show all available execute commands\n"
              << "\thelp-status                    - Show all available status values\n"
              << "\texit                           - Exit program\n";
}

void printRegisters(CommandHandler &handler)
{
    std::cout << handler.printAllRegisters() << std::endl;
}
void printExecutes(CommandHandler &handler)
{
    std::cout << handler.printAllExecutes() << std::endl;
}

void printStatuses(CommandHandler &handler)
{
    std::cout << handler.printAllStatuses() << std::endl;
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
            .sampleInterval = std::chrono::milliseconds(1),
            .bufferSize = 1024,
            .useTimestamp = true
        };
        
        Logger logger(serial, logConfig);
        CommandHandler handler(serial, logger);
        
        // Setup signal handler
        SignalHandler::reset();
        SignalHandler::setup([&logger] 
        { 
            logger.stop(); 
        });

        
        std::cout << "Motor Control Interface\n"
                  << "Type 'help' for available commands\n";

        while (!SignalHandler::shouldExit()) {
            char* line = readline("> ");
            if (line == nullptr) {  // Handle Ctrl+D
                break;
            }
            
            std::string userInput(line);
            if (!userInput.empty()) {
                add_history(line);
                
                // Handle help commands first
                if (userInput == "help") {
                    printHelp();
                }
                else if (userInput == "help-reg") {
                    printRegisters(handler);
                }
                else if (userInput == "help-exec") {
                    printExecutes(handler);
                }
                else if (userInput == "help-status") {
                    printStatuses(handler);
                }
                else if (userInput == "exit") {
                    free(line);
                    break;
                }
                else {
                    // Process other commands
                    auto result = handler.processCommand(userInput);
                    if (result.success) {
                        std::cout << result.message << std::endl;
                    } else {
                        std::cerr << "Error: " << result.message << std::endl;
                    }
                }
            }
            free(line);
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