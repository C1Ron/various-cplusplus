#include "CommandLine.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

CommandLine::CommandLine(SerialConnection& serial, FastLogger& logger)
    : m_serial(serial), m_handler(serial), m_logger(logger), m_isRunning(true) {
    registerCommands();
}

CommandLine::~CommandLine() = default;

void CommandLine::registerCommands() {
    m_commands["log-start"] = [this](const std::string&) { handleLogStart(); };
    m_commands["log-stop"] = [this](const std::string&) { handleLogStop(); };
    m_commands["log-add"] = [this](const std::string& args) { handleLogAdd(args); };
    m_commands["log-remove"] = [this](const std::string& args) { handleLogRemove(args); };
    // Register other existing commands as needed...
}

void CommandLine::processCommand(const std::string& command) {
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

void CommandLine::run() {
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

bool CommandLine::waitForInputOrShutdown(std::string& input) {
    input.clear();
    while (m_isRunning.load()) {
        if (std::getline(std::cin, input)) {
            return true;  // Input received
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Wait before checking again
    }
    return false;  // Shutdown signal received
}

void CommandLine::handleLogStart() {
    m_logger.startLogging();
    std::cout << "Logging started." << std::endl;
}

void CommandLine::handleLogStop() {
    m_logger.stopLogging();
    std::cout << "Logging stopped." << std::endl;
}

void CommandLine::handleLogAdd(const std::string& args) {
    std::istringstream iss(args);
    std::string regName;
    if (!(iss >> regName)) {
        std::cerr << "Usage: log-add <register_name>" << std::endl;
        return;
    }

    auto it = m_handler.getRegisterIdMap().find(regName);  // Access registerIdMap directly
    if (it != m_handler.getRegisterIdMap().end()) {
        ST_MPC::RegisterId regId = it->second;
        if (m_logger.addRegister(regId)) {
            std::cout << "Register " << regName << " added to logging." << std::endl;
        } else {
            std::cerr << "Failed to add register: " << regName << std::endl;
        }
    } else {
        std::cerr << "Unknown register name: " << regName << std::endl;
    }
}

void CommandLine::handleLogRemove(const std::string& args) {
    std::istringstream iss(args);
    std::string regName;
    if (!(iss >> regName)) {
        std::cerr << "Usage: log-remove <register_name>" << std::endl;
        return;
    }

    auto it = m_handler.getRegisterIdMap().find(regName);  // Access registerIdMap directly
    if (it != m_handler.getRegisterIdMap().end()) {
        ST_MPC::RegisterId regId = it->second;
        if (m_logger.removeRegister(regId)) {
            std::cout << "Register " << regName << " removed from logging." << std::endl;
        } else {
            std::cerr << "Failed to remove register: " << regName << std::endl;
        }
    } else {
        std::cerr << "Unknown register name: " << regName << std::endl;
    }
}


void CommandLine::handleExit() {
    std::cout << "Exiting..." << std::endl;
    m_isRunning.store(false);
}

void CommandLine::handleHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help               - Display this help message" << std::endl;
    std::cout << "  exit               - Exit the program" << std::endl;
    std::cout << "  log-start          - Start logging" << std::endl;
    std::cout << "  log-stop           - Stop logging" << std::endl;
    std::cout << "  log-add <reg>     - Add register to logging" << std::endl;
    std::cout << "  log-remove <reg>  - Remove register from logging" << std::endl;
    // Add any other commands here...
}


