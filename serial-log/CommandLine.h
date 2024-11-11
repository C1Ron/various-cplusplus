#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <atomic>

class CommandLine 
{
public:
    CommandLine(SerialConnection& serial, CommandHandler& handler);
    ~CommandLine();
    void run();

private:
    void processCommand(const std::string& command);
    void handleExit();   // Handle exit
    void handleHelp();   // Handle help
    bool waitForInputOrShutdown(std::string& input);  // Declaration

    SerialConnection& m_serial;
    CommandHandler& m_handler;  
    std::atomic<bool> m_isRunning;
    std::unordered_map<std::string, std::function<void(const std::string&)>> m_commands;
};

#endif // COMMAND_LINE_H
