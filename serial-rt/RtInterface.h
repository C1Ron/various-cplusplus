#ifndef RT_INTERFACE_H
#define RT_INTERFACE_H

#include "SerialConnectionRt.h"
#include "CommandHandlerRt.h"
#include "SignalHandler.h"
#include "LoggerRt.h"
#include <iostream>
#include <string>

class RtInterface
{
public:
    RtInterface(const std::string& port, unsigned int baudRate, uint8_t mscId);
    ~RtInterface();

    RtInterface(const RtInterface&) = delete;
    RtInterface& operator=(const RtInterface&) = delete;

    void run();

private:
    std::unique_ptr<SerialConnectionRt> serial;
    std::unique_ptr<LoggerRt> logger;
    std::unique_ptr<CommandHandlerRt> handler;
    uint8_t mscId;

    void setupSignalHandler();
    void cleanup();
    void processUserInput(const std::string& userInput);
    void processCommand(const std::string& command);
    void printHelp();

    static LoggerRt::LogConfig createLogConfig();
    static void printRegisters(const CommandHandlerRt& handler);
    static void printExecutes(const CommandHandlerRt& handler);
};

#endif // RT_INTERFACE_H