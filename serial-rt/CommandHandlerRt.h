#ifndef COMMAND_HANDLER_RT_H
#define COMMAND_HANDLER_RT_H

#include "SerialConnectionRt.h"
#include "FrameBuilderRt.h"
#include "FrameInterpreterRt.h"
#include "RtDefinitions.h"
#include <string>
#include <unordered_map>
#include <functional>
#include "LoggerRt.h"

class CommandHandlerRt 
{
public:
    struct CommandResult 
    {
        bool success;
        std::string message;
    };

    CommandHandlerRt(SerialConnectionRt& conn, uint8_t mscId);
    CommandHandlerRt(SerialConnectionRt& conn, uint8_t mscId, LoggerRt& logger);
    CommandResult processCommand(const std::string& command);
    const std::string printAllRegisters() const;
    const std::string printAllExecutes() const;

private:
    struct Register 
    {
        RT::RegisterId id;
        RT::RegisterType type;
    };

    // Command handlers
    CommandResult handleRead(const std::string& args);
    CommandResult handleWrite(const std::string& args);
    CommandResult handleExecute(const std::string& args);
    CommandResult handleRamp(const std::string& args);
    CommandResult handleLogStart(const std::string& args);
    CommandResult handleLogStop(const std::string& args);
    CommandResult handleLogAdd(const std::string& args);
    CommandResult handleLogRemove(const std::string& args);
    CommandResult handleLogStatus(const std::string& args);
    CommandResult handleLogConfig(const std::string& args);
    CommandResult handleError(const std::string& message, const std::exception& e) const;

    // Helper methods
    const Register& getRegister(const std::string& regName);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame, RT::RegisterType type);

    // Member variables
    SerialConnectionRt& connection;
    FrameBuilderRt frameBuilder;
    FrameInterpreterRt frameInterpreter;
    LoggerRt* logger = nullptr;
    uint8_t mscId;

    // Maps
    std::unordered_map<std::string, std::function<CommandResult(const std::string&)>> commandMap;
    std::unordered_map<std::string, Register> registerMap;
    std::unordered_map<std::string, RT::ExecuteId> executeMap;
};

#endif // COMMAND_HANDLER_RT_H