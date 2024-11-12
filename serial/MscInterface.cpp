#include "MscInterface.h"
#include <readline/readline.h>
#include <readline/history.h>

MscInterface::MscInterface(const std::string& port, unsigned int baudRate)
{
    serial = std::make_unique<SerialConnection>(port, baudRate);
    logger = std::make_unique<Logger>(*serial, createLogConfig());
    handler = std::make_unique<CommandHandler>(*serial, *logger);
    setupSignalHandler();
}

MscInterface::~MscInterface()
{
    cleanup();
}

void MscInterface::run()
{
    while (!SignalHandler::shouldExit()) {
        char* line = readline("> ");
        if (line == nullptr) {
            std::cout << "CTRL+D detected. Exiting..." << std::endl;
            break;
        }

        std::string userInput(line);
        if (!userInput.empty()) {
            add_history(line);
            processUserInput(userInput);
        }
        free(line);
    }
}

void MscInterface::setupSignalHandler()
{
    SignalHandler::reset();
    SignalHandler::setup([this] { cleanup(); });
}

void MscInterface::cleanup()
{
    if (logger) {
        std::cout << "MscInterface::cleanup() - Stopping logger..." << std::endl;
        logger->stop();
    }
}

void MscInterface::processUserInput(const std::string & userInput)
{
    if (userInput == "help") {
        printHelp();
    } else if (userInput == "help-reg") {
        printRegisters(*handler);
    } else if (userInput == "help-exec") {
        printExecutes(*handler);
    } else if (userInput == "help-status") {
        printStatuses(*handler);
    } else if (userInput == "exit") {
        std::cout << "Exiting..." << std::endl;
        SignalHandler::shouldExit(true);
    } else {
        processCommand(userInput);
    }
}

void MscInterface::printHelp()
{
    std::cout 
    << "MPC commands:========================================================================\n"
    << "\tset        <reg>    <val>       - Set register value\n"
    << "\tget        <reg>                - Get register value\n"
    << "\texec       <cmd>                - Execute command\n"
    << "\tramp       <speed>  <time>      - Execute speed ramp\n"
    << "\tcurrent    <Iq>     <Id>        - Set current references\n"
    << "Logging commands:====================================================================\n"
    << "\tlog-start                       - Start logging\n"
    << "\tlog-stop                        - Stop logging\n"
    << "\tlog-add    <reg>                - Add register to logging\n"
    << "\tlog-remove <reg>                - Remove register from logging\n"
    << "\tlog-status                      - Show logging status\n"
    << "\tlog-config  <fname> <interval>  - Update logging configuration\n"
    << "Other commands:======================================================================\n"
    << "\thelp                            - Show this help\n"
    << "\thelp-reg                        - Show all available registers and associated types\n"
    << "\thelp-exec                       - Show all available execute commands\n"
    << "\thelp-status                     - Show all available status values\n"
    << "\texit                            - Exit program\n"
    << "\tCTRL+D                          - Exit program\n";
}

void MscInterface::processCommand(const std::string& command)
{
    auto result = handler->processCommand(command);
    if (result.success) {
        std::cout << result.message << std::endl;
    } else {
        std::cerr << "Error: " << result.message << std::endl;
    }
}

Logger::LogConfig MscInterface::createLogConfig()
{
    return {
        .filename = "log.csv",
        .sampleInterval = std::chrono::milliseconds(100),
        .bufferSize = 1024,
        .useTimestamp = true
    };
}


void MscInterface::printRegisters(const CommandHandler& handler)
{
    std::cout << handler.printAllRegisters() << std::endl;
}

void MscInterface::printExecutes(const CommandHandler& handler)
{
    std::cout << handler.printAllExecutes() << std::endl;
}

void MscInterface::printStatuses(const CommandHandler& handler)
{
    std::cout << handler.printAllStatuses() << std::endl;
}
