#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include "Logger.h"

CommandHandler::CommandHandler(SerialConnection& conn) : connection(conn) 
{
    // Initialize command map
    commandMap = 
    {
        {"set", [this](const std::string& args) { return handleSetRegister(args); }},
        {"get", [this](const std::string& args) { return handleGetRegister(args); }},
        {"exec", [this](const std::string& args) { return handleExecute(args); }},
        {"ramp", [this](const std::string& args) { return handleRamp(args); }}
    };

    // Initialize register map with type information and descriptions
    registerMap = 
    {
        {"MotorId", {ST_MPC::RegisterId::TargetMotor, ST_MPC::RegisterType::UInt8}},
        {"Flags", {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32}},
        {"Status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8}},
        {"ControlMode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8}},
        {"SpeedRef", {ST_MPC::RegisterId::SpeedRef, ST_MPC::RegisterType::Int32}},
        {"SpeedKp", {ST_MPC::RegisterId::SpeedKp, ST_MPC::RegisterType::UInt16}},
        {"SpeedKi", {ST_MPC::RegisterId::SpeedKi, ST_MPC::RegisterType::UInt16}},
        {"SpeedKd", {ST_MPC::RegisterId::SpeedKd, ST_MPC::RegisterType::UInt16}},
        {"TorqueRef", {ST_MPC::RegisterId::TorqueRef, ST_MPC::RegisterType::Int16}},
        {"TorqueKp", {ST_MPC::RegisterId::TorqueKp, ST_MPC::RegisterType::UInt16}},
        {"TorqueKi", {ST_MPC::RegisterId::TorqueKi, ST_MPC::RegisterType::UInt16}},
        {"TorqueKd", {ST_MPC::RegisterId::TorqueKd, ST_MPC::RegisterType::UInt16}},
        {"FluxRef", {ST_MPC::RegisterId::FluxRef, ST_MPC::RegisterType::Int16}},
        {"FluxKp", {ST_MPC::RegisterId::FluxKp, ST_MPC::RegisterType::UInt16}},
        {"FluxKi", {ST_MPC::RegisterId::FluxKi, ST_MPC::RegisterType::UInt16}},
        {"FluxKd", {ST_MPC::RegisterId::FluxKd, ST_MPC::RegisterType::UInt16}},
        {"Status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8}},
        {"Flags",  {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32}},
        {"ControlMode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8}},
        {"MotorPower", {ST_MPC::RegisterId::MotorPower, ST_MPC::RegisterType::UInt16}},
        {"SpeedMeas", {ST_MPC::RegisterId::SpeedMeas, ST_MPC::RegisterType::Int32}},
        {"TorqueMeas", {ST_MPC::RegisterId::TorqueMeas, ST_MPC::RegisterType::Int16}},
        {"FluxMeas", {ST_MPC::RegisterId::FluxMeas, ST_MPC::RegisterType::Int16}},
        {"Ia", {ST_MPC::RegisterId::Ia, ST_MPC::RegisterType::Int16}},
        {"Ib", {ST_MPC::RegisterId::Ib, ST_MPC::RegisterType::Int16}},
        {"Ialpha", {ST_MPC::RegisterId::Ialpha, ST_MPC::RegisterType::Int16}},
        {"Ibeta", {ST_MPC::RegisterId::Ibeta, ST_MPC::RegisterType::Int16}},
        {"Iq", {ST_MPC::RegisterId::Iq, ST_MPC::RegisterType::Int16}},
        {"Id", {ST_MPC::RegisterId::Id, ST_MPC::RegisterType::Int16}},
        {"IqRef", {ST_MPC::RegisterId::IqRef, ST_MPC::RegisterType::Int16}},
        {"IdRef", {ST_MPC::RegisterId::IdRef, ST_MPC::RegisterType::Int16}},
        {"Vq", {ST_MPC::RegisterId::Vq, ST_MPC::RegisterType::Int16}},
        {"Vd", {ST_MPC::RegisterId::Vd, ST_MPC::RegisterType::Int16}},
        {"Valpha", {ST_MPC::RegisterId::Valpha, ST_MPC::RegisterType::Int16}},
        {"Vbeta", {ST_MPC::RegisterId::Vbeta, ST_MPC::RegisterType::Int16}},
        {"ElAngleMeas", {ST_MPC::RegisterId::ElAngleMeas, ST_MPC::RegisterType::Int16}},
        {"RampFinalSpeed", {ST_MPC::RegisterId::RampFinalSpeed, ST_MPC::RegisterType::Int32}},
        {"RampDuration", {ST_MPC::RegisterId::RampDuration, ST_MPC::RegisterType::UInt16}},
        {"SpeedKpDiv", {ST_MPC::RegisterId::SpeedKpDiv, ST_MPC::RegisterType::UInt16}},
        {"SpeedKiDiv", {ST_MPC::RegisterId::SpeedKiDiv, ST_MPC::RegisterType::UInt16}},
        {"TransDetReg1000", {ST_MPC::RegisterId::TransDetReg1000, ST_MPC::RegisterType::UInt8}},
        {"TransDetReg1200", {ST_MPC::RegisterId::TransDetReg1200, ST_MPC::RegisterType::UInt8}},
        {"TransDetReg1300", {ST_MPC::RegisterId::TransDetReg1300, ST_MPC::RegisterType::UInt8}},
        {"TransDetRegId", {ST_MPC::RegisterId::TransDetRegId, ST_MPC::RegisterType::UInt8}},
        {"DeadTimeRegId", {ST_MPC::RegisterId::DeadTimeRegId, ST_MPC::RegisterType::UInt8}},
        {"DeadTimeRegA", {ST_MPC::RegisterId::DeadTimeRegA, ST_MPC::RegisterType::UInt8}},
        {"DeadTimeRegB", {ST_MPC::RegisterId::DeadTimeRegB, ST_MPC::RegisterType::UInt8}},
        {"GdrPwrDis", {ST_MPC::RegisterId::GdrPwrDis, ST_MPC::RegisterType::UInt8}},
        {"GdrPwmEn", {ST_MPC::RegisterId::GdrPwmEn, ST_MPC::RegisterType::UInt8}},
        {"GdrFltPhA", {ST_MPC::RegisterId::GdrFltPhA, ST_MPC::RegisterType::UInt8}},
        {"GdrFltPhB", {ST_MPC::RegisterId::GdrFltPhB, ST_MPC::RegisterType::UInt8}},
        {"GdrFltPhC", {ST_MPC::RegisterId::GdrFltPhC, ST_MPC::RegisterType::UInt8}},
        {"GdrTempPhA", {ST_MPC::RegisterId::GdrTempPhA, ST_MPC::RegisterType::UInt32}},
        {"GdrTempPhB", {ST_MPC::RegisterId::GdrTempPhB, ST_MPC::RegisterType::UInt32}},
        {"GdrTempPhC", {ST_MPC::RegisterId::GdrTempPhC, ST_MPC::RegisterType::UInt32}},
        {"MuxRegId", {ST_MPC::RegisterId::MuxRegId, ST_MPC::RegisterType::UInt8}},
        {"TorqueKpDivPow2", {ST_MPC::RegisterId::TorqueKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"TorqueKiDivPow2", {ST_MPC::RegisterId::TorqueKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"FluxKpDivPow2", {ST_MPC::RegisterId::FluxKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"FluxKiDivPow2", {ST_MPC::RegisterId::FluxKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"SpeedKpDivPow2", {ST_MPC::RegisterId::SpeedKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"SpeedKiDivPow2", {ST_MPC::RegisterId::SpeedKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"TorqueKpDiv", {ST_MPC::RegisterId::TorqueKpDiv, ST_MPC::RegisterType::UInt16}},
        {"TorqueKiDiv", {ST_MPC::RegisterId::TorqueKiDiv, ST_MPC::RegisterType::UInt16}},
        {"FluxKpDiv", {ST_MPC::RegisterId::FluxKpDiv, ST_MPC::RegisterType::UInt16}},
        {"FluxKiDiv", {ST_MPC::RegisterId::FluxKiDiv, ST_MPC::RegisterType::UInt16}},
        {"AlignFinalFlux", {ST_MPC::RegisterId::AlignFinalFlux, ST_MPC::RegisterType::UInt16}},
        {"AlignRampUpDuration", {ST_MPC::RegisterId::AlignRampUpDuration, ST_MPC::RegisterType::UInt16}},
        {"AlignRampDownDuration", {ST_MPC::RegisterId::AlignRampDownDuration, ST_MPC::RegisterType::UInt16}},
        {"IsAligned", {ST_MPC::RegisterId::IsAligned, ST_MPC::RegisterType::UInt16}},
        {"GitVersion", {ST_MPC::RegisterId::GitVersion, ST_MPC::RegisterType::CharPtr}}

    };

    // Initialize execute command map
    executeMap = 
    {
        {"start", ST_MPC::ExecuteId::StartMotor},
        {"stop", ST_MPC::ExecuteId::StopMotor},
        {"align", ST_MPC::ExecuteId::EncoderAlign},
        {"startstop", ST_MPC::ExecuteId::StartStop},
        {"stopramp", ST_MPC::ExecuteId::StopRamp},
        {"reset", ST_MPC::ExecuteId::Reset},
        {"ping", ST_MPC::ExecuteId::Ping},
        {"faultack", ST_MPC::ExecuteId::FaultAck}
    };
}

CommandHandler::CommandHandler(SerialConnection& conn, Logger& loggerRef) : CommandHandler(conn) 
{
    logger = &loggerRef;

    // Add logger-specific commands
    commandMap["log-start"] = [this](const std::string& args) { return handleLogStart(args); };
    commandMap["log-stop"] = [this](const std::string& args) { return handleLogStop(args); };
    commandMap["log-add"] = [this](const std::string& args) { return handleLogAdd(args); };
    commandMap["log-remove"] = [this](const std::string& args) { return handleLogRemove(args); };
    commandMap["log-status"] = [this](const std::string& args) { return handleLogStatus(args); };
}

CommandHandler::CommandResult CommandHandler::processCommand(const std::string& command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    auto it = commandMap.find(cmd);
    if (it != commandMap.end()) {
        try {
            std::string args;
            std::getline(iss, args);  // Capture the rest of the line as arguments
            return it->second(args);  // Return the result of the handler
        } catch (const std::exception& e) {
            return handleError("Exception caught during command execution", e);
        }
    } else {
        return {false, "Unknown command: " + cmd + ". Type 'help' for a list of commands."};
    }
}

CommandHandler::CommandResult CommandHandler::handleSetRegister(const std::string& args)
{
    std::istringstream iss(args);
    std::string regName;
    int32_t regVal;

    if (!(iss >> regName >> regVal)) {
        return {false, "Usage: set <register_name> <value>"};
    }

    try {
        auto reg = getRegister(regName);
        auto frame = frameBuilder.buildSetRegisterFrame(1, reg.id, regVal, reg.type);
        auto response = sendAndProcessResponse(frame);
        return {true, "Register '" + regName + "' set to " + std::to_string(regVal) + "\n" + response};
    } catch (const std::exception& e) {
        return handleError("handleSetRegister failed", e);
    }
}

CommandHandler::CommandResult CommandHandler::handleGetRegister(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    if (!(iss >> regName)) {
        return {false, "Usage: get <register_name>"};
    }

    try {
        auto reg = getRegister(regName);
        auto frame = frameBuilder.buildGetRegisterFrame(1, reg.id);
        auto response = sendAndProcessResponse(frame);
        return {true, "Register '" + regName + "' response: " + response};
    }
    catch (const std::exception& e) {
        return handleError("handleGetRegister failed", e);
    }
}

CommandHandler::CommandResult CommandHandler::handleExecute(const std::string& args) 
{
    std::istringstream iss(args);
    std::string execName;
    
    if (!(iss >> execName)) {
        return {false, "Usage: exec <command_name>"};
    }

    auto execIt = executeMap.find(execName);
    if (execIt == executeMap.end()) {
        return {false, "Unknown execute command: " + execName};
    }

    auto frame = frameBuilder.buildExecuteFrame(1, execIt->second);
    auto response = sendAndProcessResponse(frame);
    return {true, "Executed command '" + execName + "'\n" + response};
}

CommandHandler::CommandResult CommandHandler::handleRamp(const std::string& args) 
{
    std::istringstream iss(args);
    int32_t finalSpeed;
    uint16_t duration;

    if (!(iss >> finalSpeed >> duration)) {
        return {false, "Usage: ramp <final_speed> <duration>"};
    }

    if (duration == 0) {
        return {false, "Duration must be greater than 0"};
    }

    try {
        auto frame = frameBuilder.buildExecuteRampFrame(1, finalSpeed, duration);
        auto response = sendAndProcessResponse(frame);
        return {true, "Started ramp to speed " + std::to_string(finalSpeed) + " over " + std::to_string(duration) + "ms\n" + response};
    } catch (const std::exception& e) {
        return handleError("handleRamp failed", e);
    }
}

CommandHandler::CommandResult CommandHandler::handleLogStart(const std::string& args) 
{
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!args.empty()) {
        return {false, "Usage: log-start"};
    }
    logger->start();
    return {true, "Logging started"};
}

CommandHandler::CommandResult CommandHandler::handleLogStop(const std::string& args) 
{
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!args.empty()) {
        return {false, "Usage: log-stop"};
    }
    logger->stop();
    return {true, "Logging stopped"};
}

CommandHandler::CommandResult CommandHandler::handleLogAdd(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;

    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!(iss >> regName)) {
        return {false, "Usage: log-add <register_name>"};
    }

    auto reg = getRegister(regName);
    
    if (logger->addRegister(regName, reg.id, reg.type)) {
        return {true, "Added register '" + regName + "' to logging"};
    }
    return {false, "Register '" + regName + "' is already being logged"};
}

CommandHandler::CommandResult CommandHandler::handleLogRemove(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;

    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!(iss >> regName)) {
        return {false, "Usage: log-remove <register_name>"};
    }
    
    if (logger->removeRegister(regName)) {
        return {true, "Removed register '" + regName + "' from logging"};
    }
    return {false, "Register '" + regName + "' was not being logged"};
}

CommandHandler::CommandResult CommandHandler::handleLogStatus(const std::string& args) 
{
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!args.empty()) {
        return {false, "Usage: log-status"};
    }
    
    std::string status = "Logging status:\n";
    status += "Running: " + std::string(logger->isRunning() ? "yes" : "no") + "\n";
    status += "Logged registers:\n";
    
    auto registers = logger->getLoggedRegisters();
    if (registers.empty()) {
        status += "  None\n";
    } else {
        for (const auto& reg : registers) {
            status += "  " + reg + "\n";
        }
    }
    
    return {true, status};
}

CommandHandler::CommandResult CommandHandler::handleError(const std::string& message, const std::exception& e) const 
{
    return {false, message + ": " + e.what()};
}

const CommandHandler::Register& CommandHandler::getRegister(const std::string& regName) 
{
    auto it = registerMap.find(regName);
    if (it == registerMap.end()) {
        throw std::runtime_error("Register not found: " + regName);
    }
    return it->second;
}

std::string CommandHandler::sendAndProcessResponse(const std::vector<uint8_t>& frame) 
{
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    frameInterpreter.printResponse(response);
    return frameInterpreter.interpretResponse(response);
}
