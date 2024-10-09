#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include <string>
#include <atomic>

class CommandLine
{
public:
    CommandLine(SerialConnection& serial);
    ~CommandLine();
    void run();

private:
    void processCommand(const std::string& command);
    void handleExit();
    void handleHelp();
    bool waitForInputOrShutdown(std::string& input);

    SerialConnection& m_serial;
    CommandHandler m_handler;
    std::atomic<bool> m_isRunning;
};

#endif // COMMAND_LINE_H
