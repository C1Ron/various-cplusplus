#include "CommandHandlerRt.h"
#include <sstream>
#include <iomanip>

CommandHandlerRt::CommandHandlerRt(SerialConnection& conn, uint8_t mscId) 
    : connection(conn), mscId(mscId)
{
    // Initialize command map
    commandMap = {
        {"read", std::bind(&CommandHandlerRt::handleRead, this, std::placeholders::_1)},
        {"write", std::bind(&CommandHandlerRt::handleWrite, this, std::placeholders::_1)},
        {"exec", std::bind(&CommandHandlerRt::handleExecute, this, std::placeholders::_1)},
        {"ramp", std::bind(&CommandHandlerRt::handleRamp, this, std::placeholders::_1)},
        {"log-start", std::bind(&CommandHandlerRt::handleLogStart, this, std::placeholders::_1)},
        {"log-stop", std::bind(&CommandHandlerRt::handleLogStop, this, std::placeholders::_1)},
        {"log-add", std::bind(&CommandHandlerRt::handleLogAdd, this, std::placeholders::_1)},
        {"log-remove", std::bind(&CommandHandlerRt::handleLogRemove, this, std::placeholders::_1)},
        {"log-status", std::bind(&CommandHandlerRt::handleLogStatus, this, std::placeholders::_1)},
        {"log-config", std::bind(&CommandHandlerRt::handleLogConfig, this, std::placeholders::_1)}
    };

    // Initialize register map
    registerMap = {
        {"ramp-final-speed", {RT::RegisterId::RAMP_FINAL_SPEED, RT::RegisterType::Int32}},
        {"ramp-duration", {RT::RegisterId::RAMP_DURATION, RT::RegisterType::UInt16}},
        {"speed-setpoint", {RT::RegisterId::SPEED_SETPOINT, RT::RegisterType::Int32}},
        {"speed-kp", {RT::RegisterId::SPEED_KP, RT::RegisterType::Float}},
        {"speed-ki", {RT::RegisterId::SPEED_KI, RT::RegisterType::Float}},
        {"speed-kd", {RT::RegisterId::SPEED_KD, RT::RegisterType::Float}},
        {"board-info", {RT::RegisterId::BOARD_INFO, RT::RegisterType::CharPtr}},
        {"current-speed", {RT::RegisterId::CURRENT_SPEED, RT::RegisterType::Float}},
        {"speed-loop-period", {RT::RegisterId::SPEED_LOOP_PERIOD_MS, RT::RegisterType::UInt32}},
        {"git-version", {RT::RegisterId::GIT_VERSION, RT::RegisterType::CharPtr}}
    };

    // Initialize execute map
    executeMap = {
        {"start", RT::ExecuteId::START_MOTOR},
        {"stop", RT::ExecuteId::STOP_MOTOR},
        {"ramp", RT::ExecuteId::RAMP_EXECUTE},
        {"feedback-start", RT::ExecuteId::START_FEEDBACK},
        {"feedback-stop", RT::ExecuteId::STOP_FEEDBACK}
    };
}

CommandHandlerRt::CommandHandlerRt(SerialConnection& conn, uint8_t mscId, LoggerRt& logger)
    : CommandHandlerRt(conn, mscId)
{
    this->logger = &logger;
}

CommandHandlerRt::CommandResult CommandHandlerRt::processCommand(const std::string& command) 
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    auto it = commandMap.find(cmd);
    if (it == commandMap.end()) {
        return {false, "Unknown command: " + cmd};
    }

    std::string args;
    std::getline(iss, args);

    try {
        return it->second(args);
    }
    catch (const std::exception& e) {
        return handleError("Command execution failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleRead(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    iss >> regName;

    if (regName.empty()) {
        return {false, "Register name required"};
    }

    try {
        const auto& reg = getRegister(regName);
        auto frame = frameBuilder.buildReadFrame(mscId, reg.id);
        std::string response = sendAndProcessResponse(frame, reg.type);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("Read failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleWrite(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    std::string valueStr;
    
    iss >> regName >> valueStr;
    
    if (regName.empty() || valueStr.empty()) {
        return {false, "Register name and value required"};
    }

    try {
        const auto& reg = getRegister(regName);
        int32_t value;

        if (reg.type == RT::RegisterType::Float) {
            float floatValue;
            std::istringstream(valueStr) >> floatValue;
            // Reinterpret float as int32_t for transmission
            value = *reinterpret_cast<int32_t*>(&floatValue);
        } else {
            std::istringstream(valueStr) >> value;
        }

        auto frame = frameBuilder.buildWriteFrame(mscId, reg.id, value, reg.type);
        std::string response = sendAndProcessResponse(frame);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("Write failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleExecute(const std::string& args) 
{
    std::istringstream iss(args);
    std::string execName;
    iss >> execName;

    if (execName.empty()) {
        return {false, "Execute command name required"};
    }

    auto it = executeMap.find(execName);
    if (it == executeMap.end()) {
        return {false, "Unknown execute command: " + execName};
    }

    try {
        auto frame = frameBuilder.buildExecuteFrame(mscId, it->second);
        std::string response = sendAndProcessResponse(frame);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("Execute failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleRamp(const std::string& args) 
{
    std::istringstream iss(args);
    std::string speedStr, timeStr;
    
    iss >> speedStr >> timeStr;
    
    if (speedStr.empty() || timeStr.empty()) {
        return {false, "Speed and time required"};
    }

    try {
        int32_t speed;
        uint16_t time;
        
        std::istringstream(speedStr) >> speed;
        std::istringstream(timeStr) >> time;

        auto frame = frameBuilder.buildRampFrame(mscId, speed, time);
        std::string response = sendAndProcessResponse(frame);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("Ramp failed", e);
    }
}

// Logging related command handlers
CommandHandlerRt::CommandResult CommandHandlerRt::handleLogStart(const std::string&) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }
    
    try {
        logger->start();
        return {true, "Logging started"};
    }
    catch (const std::exception& e) {
        return handleError("Failed to start logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogStop(const std::string&) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }
    
    try {
        logger->stop();
        return {true, "Logging stopped"};
    }
    catch (const std::exception& e) {
        return handleError("Failed to stop logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogAdd(const std::string& args) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }

    std::istringstream iss(args);
    std::string regName;
    iss >> regName;

    if (regName.empty()) {
        return {false, "Register name required"};
    }

    try {
        const auto& reg = getRegister(regName);
        if (logger->addRegister(regName, reg.id, reg.type)) {
            return {true, "Register added to logging: " + regName};
        }
        return {false, "Register already being logged: " + regName};
    }
    catch (const std::exception& e) {
        return handleError("Failed to add register to logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogRemove(const std::string& args) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }

    std::istringstream iss(args);
    std::string regName;
    iss >> regName;

    if (regName.empty()) {
        return {false, "Register name required"};
    }

    if (logger->removeRegister(regName)) {
        return {true, "Register removed from logging: " + regName};
    }
    return {false, "Register not found in logging: " + regName};
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogStatus(const std::string&) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }

    std::stringstream ss;
    ss << "Logging status:\n";
    ss << "Running: " << (logger->isRunning() ? "yes" : "no") << "\n";
    ss << "Logged registers:";
    
    auto regs = logger->getLoggedRegisters();
    if (regs.empty()) {
        ss << " none";
    } else {
        for (const auto& reg : regs) {
            ss << "\n  " << reg;
        }
    }

    return {true, ss.str()};
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogConfig(const std::string& args) 
{
    if (!logger) {
        return {false, "Logger not initialized"};
    }

    std::istringstream iss(args);
    std::string filename;
    int interval;
    
    iss >> filename >> interval;
    
    if (filename.empty() || interval <= 0) {
        return {false, "Filename and positive interval required"};
    }

    try {
        auto config = logger->getConfig();
        config.filename = filename;
        config.sampleInterval = std::chrono::milliseconds(interval);
        logger->setConfig(config);
        return {true, "Logger configuration updated"};
    }
    catch (const std::exception& e) {
        return handleError("Failed to update logger configuration", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleError(const std::string& message, 
                                                            const std::exception& e) const 
{
    return {false, message + ": " + e.what()};
}

const CommandHandlerRt::Register& CommandHandlerRt::getRegister(const std::string& regName) 
{
    auto it = registerMap.find(regName);
    if (it == registerMap.end()) {
        throw std::runtime_error("Unknown register: " + regName);
    }
    return it->second;
}

std::string CommandHandlerRt::sendAndProcessResponse(const std::vector<uint8_t>& frame) 
{
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    return frameInterpreter.interpretResponse(response);
}

std::string CommandHandlerRt::sendAndProcessResponse(const std::vector<uint8_t>& frame, 
                                                   RT::RegisterType type) 
{
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    return frameInterpreter.interpretResponse(response, type);
}

const std::string CommandHandlerRt::printAllRegisters() const 
{
    std::stringstream ss;
    ss << "Available registers:\n";
    for (const auto& reg : registerMap) {
        ss << "  " << std::setw(20) << std::left << reg.first;
        switch (reg.second.type) {
            case RT::RegisterType::UInt8: ss << "UINT8"; break;
            case RT::RegisterType::Int16: ss << "INT16"; break;
            case RT::RegisterType::UInt16: ss << "UINT16"; break;
            case RT::RegisterType::Int32: ss << "INT32"; break;
            case RT::RegisterType::UInt32: ss << "UINT32"; break;
            case RT::RegisterType::Float: ss << "FLOAT"; break;
            case RT::RegisterType::CharPtr: ss << "STRING"; break;
        }
        ss << "\n";
    }
    return ss.str();
}

const std::string CommandHandlerRt::printAllExecutes() const 
{
    std::stringstream ss;
    ss << "Available execute commands:\n";
    for (const auto& exec : executeMap) {
        ss << "  " << exec.first << "\n";
    }
    return ss.str();
}