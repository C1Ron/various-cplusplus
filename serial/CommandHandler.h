#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "SerialConnection.h"
#include "FrameBuilder.h"
#include "FrameInterpreter.h"
#include "StMpcDefinitions.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <vector>
#include "Logger.h"

class CommandHandler {
public:
    struct CommandResult {
        bool success;
        std::string message;
    };

    CommandHandler(SerialConnection& conn);
    CommandHandler(SerialConnection& conn, Logger& logger);
    CommandResult processCommand(const std::string& command);

private:
    struct RegisterInfo {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
        std::string description;
    };

    // Command handlers
    CommandResult handleSetRegister(const std::vector<std::string>& args);
    CommandResult handleGetRegister(const std::vector<std::string>& args);
    CommandResult handleExecute(const std::vector<std::string>& args);
    CommandResult handleRamp(const std::vector<std::string>& args);
    CommandResult handleLogStart(const std::vector<std::string>& args);
    CommandResult handleLogStop(const std::vector<std::string>& args);
    CommandResult handleLogAdd(const std::vector<std::string>& args);
    CommandResult handleLogRemove(const std::vector<std::string>& args);
    CommandResult handleLogStatus(const std::vector<std::string>& args);

    // Helper methods
    std::vector<std::string> parseArgs(const std::string& command);
    std::optional<RegisterInfo> getRegisterInfo(const std::string& regName);
    std::string sendAndProcessResponse(const std::vector<uint8_t>& frame);

    // Member variables
    SerialConnection& connection;
    FrameBuilder frameBuilder;
    FrameInterpreter frameInterpreter;
    Logger* logger = nullptr;

    // Maps
    std::unordered_map<std::string, std::function<CommandResult(const std::vector<std::string>&)>> commandMap;
    std::unordered_map<std::string, RegisterInfo> registerMap;
    std::unordered_map<std::string, ST_MPC::ExecuteId> executeMap;
};

#endif // COMMAND_HANDLER_H