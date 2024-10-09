#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "SerialConnection.h"
#include "FrameBuilder.h"
#include "FrameInterpreter.h"
#include "StMpcDefinitions.h"
#include <string>
#include <unordered_map>
#include <functional>

class CommandHandler
{
public:
    CommandHandler(SerialConnection& conn);
    ~CommandHandler();
    void processUserCommand(const std::string& command);
    const std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType>& getRegisterTypeMap() const 
    { 
        return registerTypeMap; 
    }

private:
    void initializeMaps();
    void handleSetRegister(const std::string& args);
    void handleGetRegister(const std::string& args);
    void handleExecute(const std::string& args);
    void handleExecuteRamp(const std::string& args);
    void sendAndProcessResponse(const std::vector<uint8_t>& frame);

    SerialConnection& connection;
    FrameBuilder frameBuilder;
    FrameInterpreter frameInterpreter;

    std::unordered_map<std::string, ST_MPC::RegisterId> registerIdMap;
    std::unordered_map<std::string, ST_MPC::ExecuteId> executeIdMap;
    std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType> registerTypeMap;
    std::unordered_map<std::string, std::function<void(const std::string&)>> m_commandMap;


};

#endif // COMMAND_HANDLER_H
