#include "CommandLine.h"
#include "SignalHandler.h"
#include <iostream>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>

CommandLine::CommandLine(SerialConnection& serial)
    : m_serial(serial), m_handler(serial), m_isRunning(true)
{
    SignalHandler::registerHandler();
}

CommandLine::~CommandLine() = default;

void CommandLine::run()
{
    std::string userInput;
    while (m_isRunning.load() && !SignalHandler::shouldShutdown()) {
        std::cout << "> " << std::flush;

        if (waitForInputOrShutdown(userInput)) {
            std::cout << "Received input: " << userInput << std::endl;
            processCommand(userInput);
        } else {
            std::cout << "\nExiting due to shutdown signal." << std::endl;
            break;
        }
    }
    std::cout << "CommandLine run loop ended" << std::endl;
}


bool CommandLine::waitForInputOrShutdown(std::string& input)
{
    input.clear();
    while (m_isRunning.load() && !SignalHandler::shouldShutdown()) {
        // Set up the file descriptor set for select
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);

        // Set a timeout for select
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;  // 100 milliseconds

        int ret = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout);
        
        if (ret > 0 && FD_ISSET(STDIN_FILENO, &fds)) {
            // Input is ready on stdin
            if (std::getline(std::cin, input)) {
                return true;
            }
            if (std::cin.eof()) {
                m_isRunning.store(false);
                break;
            }
        } else if (ret == 0) {
            // Timeout occurred: check shutdown flag and loop again
            if (SignalHandler::shouldShutdown()) {
                m_isRunning.store(false);
                break;
            }
        } else {
            std::cerr << "Error with select() on std::cin." << std::endl;
            m_isRunning.store(false);
            break;
        }
    }

    return false;
}

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
            m_handler.processUserCommand(command);  // Delegate other commands to CommandHandler
        } catch (const std::exception& e) {
            std::cerr << "Exception caught during command execution: " << e.what() << std::endl;
        }
    }
}

void CommandLine::handleExit()
{
    std::cout << "Exiting..." << std::endl;
    m_isRunning.store(false);
}

void CommandLine::handleHelp()
{
    std::cout << "Available commands:" << std::endl;
    std::cout << "  set <reg> <value>  - Set register value" << std::endl;
    std::cout << "  get <reg>          - Get register value" << std::endl;
    std::cout << "  exec <command>     - Execute a command" << std::endl;
    std::cout << "  ramp <speed> <dur> - Execute speed ramp" << std::endl;
    std::cout << "  help               - Display this help message" << std::endl;
    std::cout << "  exit               - Exit the program" << std::endl;
}
