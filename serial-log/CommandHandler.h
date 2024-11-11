// CommandHandler.h
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "SerialConnection.h"
#include "FrameBuilder.h"
#include "FrameInterpreter.h"
#include "FastLogger.h"
#include "StMpcDefinitions.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

// Forward declare FastLogger
class FastLogger;

class CommandHandler {
public:
    CommandHandler(SerialConnection& conn);
    ~CommandHandler();
    
    // Main command processing
    void processUserCommand(const std::string& command);
    
    // Attach logger after construction
    void attachLogger(std::shared_ptr<FastLogger> logger) { m_logger = logger; }
    
    // Getters for register information
    const std::unordered_map<std::string, ST_MPC::RegisterId>& getRegisterIdMap() const { return registerIdMap; }
    const std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType>& getRegisterTypeMap() const { return registerTypeMap; }

private:
    // STMPC command handlers
    void handleSetRegister(const std::string& args);
    void handleGetRegister(const std::string& args);
    void handleExecute(const std::string& args);
    void handleExecuteRamp(const std::string& args);
    
    // Logging command handlers
    void handleLogStart(const std::string& args);
    void handleLogStop(const std::string& args);
    void handleLogAdd(const std::string& args);
    void handleLogRemove(const std::string& args);
    
    void initializeMaps();
    void sendAndProcessResponse(const std::vector<uint8_t>& frame);

    SerialConnection& connection;
    FrameBuilder frameBuilder;
    FrameInterpreter frameInterpreter;
    std::shared_ptr<FastLogger> m_logger;  // Use shared_ptr to avoid ownership issues

    std::unordered_map<std::string, ST_MPC::RegisterId> registerIdMap;
    std::unordered_map<std::string, ST_MPC::ExecuteId> executeIdMap;
    std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType> registerTypeMap;
    std::unordered_map<std::string, std::function<void(const std::string&)>> m_commandMap;
};

#endif // COMMAND_HANDLER_H