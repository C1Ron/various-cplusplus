// CommandHandler.h
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "SerialConnection.h"
#include "FrameBuilder.h"
#include "FrameInterpreter.h"

class CommandHandler 
{
public:
    CommandHandler(SerialConnection& conn);
    void processUserCommand(const std::string& command);

private:
    SerialConnection& connection;
    FrameBuilder frameBuilder;
    FrameInterpreter frameInterpreter;
};

#endif // COMMAND_HANDLER_H
