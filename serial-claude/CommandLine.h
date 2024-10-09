#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include "FastLogger.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <atomic>

class CommandLine {
public:
    CommandLine(SerialConnection& serial, FastLogger& logger);
    ~CommandLine();
    void run();

private:
    void registerCommands();
    void processCommand(const std::string& command);
    void handleLogStart();
    void handleLogStop();
    void handleLogAdd(const std::string& args);
    void handleLogRemove(const std::string& args);
    void handleExit();   // Handle exit
    void handleHelp();   // Handle help
    ST_MPC::RegisterId getRegisterIdFromName(const std::string& regName) const;  // Declaration
    bool waitForInputOrShutdown(std::string& input);  // Declaration

    SerialConnection& m_serial;
    CommandHandler m_handler;  
    FastLogger& m_logger; 
    std::atomic<bool> m_isRunning;
    std::unordered_map<std::string, std::function<void(const std::string&)>> m_commands;
};

#endif // COMMAND_LINE_H
