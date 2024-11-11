#include "CommandLine.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

CommandLine::CommandLine(SerialConnection& serial, CommandHandler& handler)
    : m_serial(serial), m_handler(handler), m_isRunning(true) 
{
    // intentionally empty
}

CommandLine::~CommandLine() = default;


void CommandLine::processCommand(const std::string& command) 
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "exit") {
        handleExit();
    } else if (cmd == "help") {
        handleHelp();
    } else {
        try {
            // Check if command exists in the registered commands
            auto it = m_commands.find(cmd);
            if (it != m_commands.end()) {
                it->second(command.substr(command.find(' ') + 1));  // Pass the rest of the command
            } else {
                m_handler.processUserCommand(command);  // Delegate other commands to CommandHandler
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception caught during command execution: " << e.what() << std::endl;
        }
    }
}

void CommandLine::run() 
{
    std::string userInput;
    while (m_isRunning.load()) {
        std::cout << "> " << std::flush;

        // Wait for user input and process command
        if (waitForInputOrShutdown(userInput)) {
            std::cout << "Received input: " << userInput << std::endl;
            processCommand(userInput);
        }
    }
    std::cout << "CommandLine run loop ended" << std::endl;
}

bool CommandLine::waitForInputOrShutdown(std::string& input) 
{
    input.clear();
    while (m_isRunning.load()) {
        if (std::getline(std::cin, input)) {
            return true;  // Input received
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait before checking again
    }
    return false;  // Shutdown signal received
}


void CommandLine::handleExit() 
{
    std::cout << "Exiting..." << std::endl;
    m_isRunning.store(false);
}

void CommandLine::handleHelp() 
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help                        - Display this help message" << std::endl;
    std::cout << "  exit                        - Exit the program" << std::endl;
    std::cout << "  get  <reg>                  - Get register" << std::endl;
    std::cout << "  set  <reg> <val>            - Set register" << std::endl;
    std::cout << "  exec <cmd>                  - Execute cmd" << std::endl;
    std::cout << "  ramp <speed> <duration>     - Execute ramp" << std::endl;
    std::cout << "  log-start                   - Start logging" << std::endl;
    std::cout << "  log-stop                    - Stop logging" << std::endl;
    std::cout << "  log-add <reg>               - Add register to logging" << std::endl;
    std::cout << "  log-remove <reg>            - Remove register from logging" << std::endl;
}


