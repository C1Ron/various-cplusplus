#include "CommandHandler.h"
#include <iostream>
#include <sstream>

CommandHandler::CommandHandler(SerialConnection& conn)
    : connection(conn)
{
    initializeMaps();
}

CommandHandler::~CommandHandler() = default;

void CommandHandler::initializeMaps()
{
    m_commandMap["set"] = [this](const std::string& args) { handleSetRegister(args); };
    m_commandMap["get"] = [this](const std::string& args) { handleGetRegister(args); };
    m_commandMap["exec"] = [this](const std::string& args) { handleExecute(args); };
    m_commandMap["ramp"] = [this](const std::string& args) { handleExecuteRamp(args); };
    m_commandMap["log-add"] = [this](const std::string& args) { handleLogAdd(args); };
    m_commandMap["log-remove"] = [this](const std::string& args) { handleLogRemove(args); };
    m_commandMap["log-start"] = [this](const std::string& args) { handleLogStart(args); };
    m_commandMap["log-stop"] = [this](const std::string& args) { handleLogStop(args); };

    registerIdMap = {
        {"Flags", ST_MPC::RegisterId::Flags},
        {"Status", ST_MPC::RegisterId::Status},
        {"ControlMode", ST_MPC::RegisterId::ControlMode},
        {"SpeedRef", ST_MPC::RegisterId::SpeedRef},
        {"SpeedKp", ST_MPC::RegisterId::SpeedKp},
        {"SpeedKi", ST_MPC::RegisterId::SpeedKi},
        {"SpeedKd", ST_MPC::RegisterId::SpeedKd},
        {"TorqueRef", ST_MPC::RegisterId::TorqueRef},
        {"TorqueKp", ST_MPC::RegisterId::TorqueKp},
        {"TorqueKi", ST_MPC::RegisterId::TorqueKi},
        {"TorqueKd", ST_MPC::RegisterId::TorqueKd},
        {"FluxRef", ST_MPC::RegisterId::FluxRef},
        {"FluxKp", ST_MPC::RegisterId::FluxKp},
        {"FluxKi", ST_MPC::RegisterId::FluxKi},
        {"FluxKd", ST_MPC::RegisterId::FluxKd},
        {"MotorPower", ST_MPC::RegisterId::MotorPower},
        {"SpeedMeas", ST_MPC::RegisterId::SpeedMeas},
        {"TorqueMeas", ST_MPC::RegisterId::TorqueMeas},
        {"FluxMeas", ST_MPC::RegisterId::FluxMeas},
        {"RampFinalSpeed", ST_MPC::RegisterId::RampFinalSpeed},
        {"RampDuration", ST_MPC::RegisterId::RampDuration}
    };

    executeIdMap = {
        {"StartMotor", ST_MPC::ExecuteId::StartMotor},
        {"StopMotor", ST_MPC::ExecuteId::StopMotor},
        {"EncoderAlign", ST_MPC::ExecuteId::EncoderAlign},
        {"StartStop", ST_MPC::ExecuteId::StartStop},
        {"StopRamp", ST_MPC::ExecuteId::StopRamp},
        {"Reset", ST_MPC::ExecuteId::Reset},
        {"Ping", ST_MPC::ExecuteId::Ping},
        {"FaultAck", ST_MPC::ExecuteId::FaultAck}
    };

    registerTypeMap = {
        {ST_MPC::RegisterId::TargetMotor, ST_MPC::RegisterType::UInt8},
        {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32},
        {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8},
        {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8},
        {ST_MPC::RegisterId::SpeedRef, ST_MPC::RegisterType::Int32},
        {ST_MPC::RegisterId::SpeedKp, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::SpeedKi, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::SpeedKd, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::TorqueRef, ST_MPC::RegisterType::Int16},
        {ST_MPC::RegisterId::TorqueKp, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::TorqueKi, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::TorqueKd, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::FluxRef, ST_MPC::RegisterType::Int16},
        {ST_MPC::RegisterId::FluxKp, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::FluxKi, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::FluxKd, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::MotorPower, ST_MPC::RegisterType::UInt16},
        {ST_MPC::RegisterId::SpeedMeas, ST_MPC::RegisterType::Int32},
        {ST_MPC::RegisterId::TorqueMeas, ST_MPC::RegisterType::Int16},
        {ST_MPC::RegisterId::FluxMeas, ST_MPC::RegisterType::Int16},
        {ST_MPC::RegisterId::RampFinalSpeed, ST_MPC::RegisterType::Int32},
        {ST_MPC::RegisterId::RampDuration, ST_MPC::RegisterType::UInt16}
    };
}

void CommandHandler::processUserCommand(const std::string& command)
{
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    auto it = m_commandMap.find(cmd);
    if (it != m_commandMap.end()) {
        try {
            it->second(command.substr(command.find(cmd) + cmd.length()));
        } catch (const std::exception& e) {
            std::cerr << "Exception caught during command execution: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << cmd << ". Type 'help' for a list of commands." << std::endl;
    }
}

void CommandHandler::handleSetRegister(const std::string& args)
{
    std::istringstream iss(args);
    std::string regName;
    int32_t regVal;

    if (!(iss >> regName >> regVal)) {
        std::cerr << "Usage: set <register_name> <value>" << std::endl;
        return;
    }

    auto regIdIt = registerIdMap.find(regName);
    if (regIdIt == registerIdMap.end()) {
        std::cerr << "Unknown register name: " << regName << std::endl;
        return;
    }

    ST_MPC::RegisterId regID = regIdIt->second;
    std::vector<uint8_t> frame = frameBuilder.buildSetRegisterFrame(1, regID, regVal, registerTypeMap[regID]);
    sendAndProcessResponse(frame);
}

void CommandHandler::handleGetRegister(const std::string& args)
{
    std::istringstream iss(args);
    std::string regName;

    if (!(iss >> regName)) {
        std::cerr << "Usage: get <register_name>" << std::endl;
        return;
    }

    auto regIdIt = registerIdMap.find(regName);
    if (regIdIt == registerIdMap.end()) {
        std::cerr << "Unknown register name: " << regName << std::endl;
        return;
    }

    ST_MPC::RegisterId regID = regIdIt->second;
    std::vector<uint8_t> frame = frameBuilder.buildGetRegisterFrame(1, regID);
    sendAndProcessResponse(frame);
}

void CommandHandler::handleExecute(const std::string& args)
{
    std::istringstream iss(args);
    std::string execName;

    if (!(iss >> execName)) {
        std::cerr << "Usage: exec <command_name>" << std::endl;
        return;
    }

    auto execIdIt = executeIdMap.find(execName);
    if (execIdIt == executeIdMap.end()) {
        std::cerr << "Unknown execute command: " << execName << std::endl;
        return;
    }

    ST_MPC::ExecuteId execID = execIdIt->second;
    std::vector<uint8_t> frame = frameBuilder.buildExecuteFrame(1, execID);
    sendAndProcessResponse(frame);
}

void CommandHandler::handleExecuteRamp(const std::string& args)
{
    std::istringstream iss(args);
    int32_t finalSpeed;
    uint16_t duration;

    if (!(iss >> finalSpeed >> duration)) {
        std::cerr << "Usage: ramp <final_speed> <duration>" << std::endl;
        return;
    }

    std::vector<uint8_t> frame = frameBuilder.buildExecuteRampFrame(1, finalSpeed, duration);
    sendAndProcessResponse(frame);
}

void CommandHandler::handleLogStart(const std::string& args) 
{
    m_logger->startLogging();
    std::cout << "Logging started." << std::endl;
}

void CommandHandler::handleLogStop(const std::string& args) 
{
    m_logger->stopLogging();
    std::cout << "Logging stopped." << std::endl;
}

void CommandHandler::handleLogAdd(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    if (!(iss >> regName)) {
        std::cerr << "Usage: log-add <register_name>" << std::endl;
        return;
    }

    auto it = getRegisterIdMap().find(regName);  // Access registerIdMap directly
    if (it != getRegisterIdMap().end()) {
        ST_MPC::RegisterId regId = it->second;
        if (m_logger->addRegister(regId)) {
            std::cout << "Register " << regName << " added to logging." << std::endl;
        } else {
            std::cerr << "Failed to add register: " << regName << std::endl;
        }
    } else {
        std::cerr << "Unknown register name: " << regName << std::endl;
    }
}

void CommandHandler::handleLogRemove(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    if (!(iss >> regName)) {
        std::cerr << "Usage: log-remove <register_name>" << std::endl;
        return;
    }

    auto it = getRegisterIdMap().find(regName);  // Access registerIdMap directly
    if (it != getRegisterIdMap().end()) {
        ST_MPC::RegisterId regId = it->second;
        if (m_logger->removeRegister(regId)) {
            std::cout << "Register " << regName << " removed from logging." << std::endl;
        } else {
            std::cerr << "Failed to remove register: " << regName << std::endl;
        }
    } else {
        std::cerr << "Unknown register name: " << regName << std::endl;
    }
}
void CommandHandler::sendAndProcessResponse(const std::vector<uint8_t>& frame)
{
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    frameInterpreter.printResponse(response);
    std::cout << frameInterpreter.interpretResponse(response) << std::endl;
}
