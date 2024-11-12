#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "SerialConnection.h"
#include "FrameBuilder.h"
#include "FrameInterpreter.h"
#include "StMpcDefinitions.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include "Logger.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

class CommandHandler 
{
public:
    struct CommandResult 
    {
        bool success;
        std::string message;
    };

    CommandHandler(SerialConnection& conn);
    CommandHandler(SerialConnection& conn, Logger& logger);
    CommandResult processCommand(const std::string& command);
    const std::string printAllRegisters() const;
    const std::string printAllExecutes() const;
    const std::string printAllStatuses() const;

private:
    struct Register 
    {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
    };

    // Command handlers
    CommandResult handleSetRegister(const std::string& args);
    CommandResult handleGetRegister(const std::string& args);
    CommandResult handleExecute(const std::string& args);
    CommandResult handleRamp(const std::string& args);
    CommandResult handleCurrentRef(const std::string& args);
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
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame, ST_MPC::RegisterType type);

    // Member variables
    SerialConnection& connection;
    FrameBuilder frameBuilder;
    FrameInterpreter frameInterpreter;
    Logger* logger = nullptr;

    // Maps
    std::unordered_map<std::string, std::function<CommandResult(const std::string&)>> commandMap;
    std::unordered_map<std::string, Register> registerMap;
    std::unordered_map<std::string, ST_MPC::ExecuteId> executeMap;
    std::unordered_map<std::string, ST_MPC::Status> statusMap;

    pid_t plotterPid{0};  // PID of plotter process
    void startPlot();
    void stopPlot();
};

#endif // COMMAND_HANDLER_H
