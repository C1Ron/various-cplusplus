#ifndef COMMAND_HANDLER_RT_H
#define COMMAND_HANDLER_RT_H

#include "SerialConnectionRt.h"
#include "FrameBuilderRt.h"
#include "FrameInterpreterRt.h"
#include "RtDefinitions.h"
#include "StMpcDefinitions.h"
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
    const std::string printAllFocRegisters() const;
    const std::string printAllFocExecutes() const;

private:
    struct Register 
    {
        RT::RegisterId id;
        RT::RegisterType type;
    };
    struct FocRegister
    {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
    };

    // Command handlers
    CommandResult handleRead(const std::string& args);
    CommandResult handleWrite(const std::string& args);
    CommandResult handleExecute(const std::string& args);
    CommandResult handleFoc(const std::string& args);
    CommandResult handleFocRead(const std::string& args);
    CommandResult handleFocWrite(const std::string& args);
    CommandResult handleFocExecute(const std::string& args);

    CommandResult handleLogStart(const std::string& args);
    CommandResult handleLogStop(const std::string& args);

    CommandResult handleLogAddRt(const std::string& args);
    CommandResult handleLogRemoveRt(const std::string& args);
    CommandResult handleLogAddFoc(const std::string& args);
    CommandResult handleLogRemoveFoc(const std::string& args);

    CommandResult handleLogStatus(const std::string& args);
    CommandResult handleLogConfig(const std::string& args);
    CommandResult handleError(const std::string& message, const std::exception& e) const;

    // Helper methods
    const Register& getRegister(const std::string& regName);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame, RT::RegisterType type);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame, ST_MPC::RegisterType type);

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
    std::unordered_map<std::string, FocRegister> focRegisterMap;
    std::unordered_map<std::string, ST_MPC::ExecuteId> focExecuteMap;
};

#endif // COMMAND_HANDLER_RT_H