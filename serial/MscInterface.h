#ifndef MSC_INTERFACE_H
#define MSC_INTERFACE_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include "SignalHandler.h"
#include "Logger.h"
#include <iostream>
#include <string>

class MscInterface
{
public:
    MscInterface(const std::string& port, unsigned int baudRate);
    ~MscInterface();

    // Delete copy operations to prevent multiple instances of the interface
    // Good practice as the class manages unique resources (e.g. serial port, logger)
    MscInterface(const MscInterface&) = delete;
    MscInterface& operator=(const MscInterface&) = delete;

    void run();

private:
    // Member variables (unique_ptr for automatic resource management)
    std::unique_ptr<SerialConnection> serial;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<CommandHandler> handler;

    // Private helper methods
    void setupSignalHandler();
    void cleanup();
    void processUserInput(const std::string& userInput);
    void processCommand(const std::string& command);
    void printHelp();

    // Static helper methods
    static Logger::LogConfig createLogConfig();
    static void printRegisters(const CommandHandler& handler);
    static void printExecutes(const CommandHandler& handler);
    static void printStatuses(const CommandHandler& handler);

};

#endif // MSC_INTERFACE_H