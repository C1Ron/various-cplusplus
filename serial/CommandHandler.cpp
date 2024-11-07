#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include "Logger.h"

CommandHandler::CommandHandler(SerialConnection& conn) : connection(conn) {
    // Initialize command map
    commandMap = {
        {"set", [this](const auto& args) { return handleSetRegister(args); }},
        {"get", [this](const auto& args) { return handleGetRegister(args); }},
        {"exec", [this](const auto& args) { return handleExecute(args); }},
        {"ramp", [this](const auto& args) { return handleRamp(args); }}
    };

    // Initialize register map with type information and descriptions
    registerMap = {
        {"SpeedRef", {ST_MPC::RegisterId::SpeedRef, ST_MPC::RegisterType::Int32, "Speed reference value"}},
        {"SpeedKp", {ST_MPC::RegisterId::SpeedKp, ST_MPC::RegisterType::UInt16, "Speed controller proportional gain"}},
        {"SpeedKi", {ST_MPC::RegisterId::SpeedKi, ST_MPC::RegisterType::UInt16, "Speed controller integral gain"}},
        {"SpeedKd", {ST_MPC::RegisterId::SpeedKd, ST_MPC::RegisterType::UInt16, "Speed controller derivative gain"}},
        {"TorqueRef", {ST_MPC::RegisterId::TorqueRef, ST_MPC::RegisterType::Int16, "Torque reference value"}},
        {"TorqueKp", {ST_MPC::RegisterId::TorqueKp, ST_MPC::RegisterType::UInt16, "Torque controller proportional gain"}},
        {"TorqueKi", {ST_MPC::RegisterId::TorqueKi, ST_MPC::RegisterType::UInt16, "Torque controller integral gain"}},
        {"TorqueKd", {ST_MPC::RegisterId::TorqueKd, ST_MPC::RegisterType::UInt16, "Torque controller derivative gain"}},
        {"FluxRef", {ST_MPC::RegisterId::FluxRef, ST_MPC::RegisterType::Int16, "Flux reference value"}},
        {"FluxKp", {ST_MPC::RegisterId::FluxKp, ST_MPC::RegisterType::UInt16, "Flux controller proportional gain"}},
        {"FluxKi", {ST_MPC::RegisterId::FluxKi, ST_MPC::RegisterType::UInt16, "Flux controller integral gain"}},
        {"FluxKd", {ST_MPC::RegisterId::FluxKd, ST_MPC::RegisterType::UInt16, "Flux controller derivative gain"}},
        {"Status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8, "Motor status"}},
        {"Flags",  {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32, "Status flags"}},
        {"ControlMode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8, "Control mode selection"}},
        {"MotorPower", {ST_MPC::RegisterId::MotorPower, ST_MPC::RegisterType::UInt16, "Motor power measurement"}},
        {"SpeedMeas", {ST_MPC::RegisterId::SpeedMeas, ST_MPC::RegisterType::Int32, "Measured speed"}},
        {"TorqueMeas", {ST_MPC::RegisterId::TorqueMeas, ST_MPC::RegisterType::Int16, "Measured torque"}},
        {"FluxMeas", {ST_MPC::RegisterId::FluxMeas, ST_MPC::RegisterType::Int16, "Measured flux"}},
        {"RampFinalSpeed", {ST_MPC::RegisterId::RampFinalSpeed, ST_MPC::RegisterType::Int32, "Final speed for ramping"}},
        {"RampDuration", {ST_MPC::RegisterId::RampDuration, ST_MPC::RegisterType::UInt16, "Duration for speed ramping"}}
    };
    // Initialize execute command map
    executeMap = {
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

// Alternative constructor for initializing with a Logger instance
CommandHandler::CommandHandler(SerialConnection& conn, Logger& logger) : CommandHandler(conn) 
{
    this->logger = &logger;

    // Add logger-specific commands
    commandMap["logstart"] = [this](const auto& args) { return handleLogStart(args); };
    commandMap["logstop"] = [this](const auto& args) { return handleLogStop(args); };
    commandMap["logadd"] = [this](const auto& args) { return handleLogAdd(args); };
    commandMap["logremove"] = [this](const auto& args) { return handleLogRemove(args); };
    commandMap["logstatus"] = [this](const auto& args) { return handleLogStatus(args); };
}

CommandHandler::CommandResult CommandHandler::processCommand(const std::string& command) {
    try {
        auto args = parseArgs(command);
        if (args.empty()) {
            return {false, "Empty command"};
        }

        auto cmdIt = commandMap.find(args[0]);
        if (cmdIt == commandMap.end()) {
            return {false, "Unknown command: " + args[0]};
        }

        args.erase(args.begin());  // Remove command name
        return cmdIt->second(args);
    }
    catch (const std::exception& e) {
        return {false, std::string("Error processing command: ") + e.what()};
    }
}

CommandHandler::CommandResult CommandHandler::handleSetRegister(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return {false, "Usage: set <register_name> <value>"};
    }

    auto regInfo = getRegisterInfo(args[0]);
    if (!regInfo) {
        return {false, "Unknown register: " + args[0]};
    }

    try {
        int32_t value = std::stoi(args[1]);
        auto frame = frameBuilder.buildSetRegisterFrame(1, regInfo->id, value, regInfo->type);
        auto response = sendAndProcessResponse(frame);
        return {true, "Register '" + args[0] + "' set to " + args[1] + "\n" + response};
    }
    catch (const std::exception& e) {
        return {false, "Invalid value format: " + std::string(e.what())};
    }
}

CommandHandler::CommandResult CommandHandler::handleGetRegister(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return {false, "Usage: get <register_name>"};
    }

    auto regInfo = getRegisterInfo(args[0]);
    if (!regInfo) {
        return {false, "Unknown register: " + args[0]};
    }

    auto frame = frameBuilder.buildGetRegisterFrame(1, regInfo->id);
    auto response = sendAndProcessResponse(frame);
    return {true, "Register '" + args[0] + "' response: " + response};
}

CommandHandler::CommandResult CommandHandler::handleExecute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return {false, "Usage: exec <command_name>"};
    }

    auto execIt = executeMap.find(args[0]);
    if (execIt == executeMap.end()) {
        return {false, "Unknown execute command: " + args[0]};
    }

    auto frame = frameBuilder.buildExecuteFrame(1, execIt->second);
    auto response = sendAndProcessResponse(frame);
    return {true, "Executed command '" + args[0] + "'\n" + response};
}

CommandHandler::CommandResult CommandHandler::handleRamp(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return {false, "Usage: ramp <final_speed> <duration>"};
    }

    try {
        int32_t finalSpeed = std::stoi(args[0]);
        uint16_t duration = static_cast<uint16_t>(std::stoi(args[1]));

        if (duration == 0) {
            return {false, "Duration must be greater than 0"};
        }

        auto frame = frameBuilder.buildExecuteRampFrame(1, finalSpeed, duration);
        auto response = sendAndProcessResponse(frame);
        return {true, "Started ramp to speed " + args[0] + " over " + args[1] + "ms\n" + response};
    }
    catch (const std::exception& e) {
        return {false, std::string("Invalid argument format: ") + e.what()};
    }
}

std::vector<std::string> CommandHandler::parseArgs(const std::string& command) {
    std::vector<std::string> args;
    std::istringstream iss(command);
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    return args;
}

std::optional<CommandHandler::RegisterInfo> CommandHandler::getRegisterInfo(const std::string& regName) {
    auto it = registerMap.find(regName);
    if (it != registerMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string CommandHandler::sendAndProcessResponse(const std::vector<uint8_t>& frame) {
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    frameInterpreter.printResponse(response);
    return frameInterpreter.interpretResponse(response);
}

// Implement the new command handlers related to logging
CommandHandler::CommandResult CommandHandler::handleLogStart(const std::vector<std::string>& args) {
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!args.empty()) {
        return {false, "Usage: log-start"};
    }
    logger->start();
    return {true, "Logging started"};
}

CommandHandler::CommandResult CommandHandler::handleLogStop(const std::vector<std::string>& args) {
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (!args.empty()) {
        return {false, "Usage: log-stop"};
    }
    logger->stop();
    return {true, "Logging stopped"};
}

CommandHandler::CommandResult CommandHandler::handleLogAdd(const std::vector<std::string>& args) {
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (args.size() != 1) {
        return {false, "Usage: log-add <register_name>"};
    }
    
    auto regInfo = getRegisterInfo(args[0]);
    if (!regInfo) {
        return {false, "Unknown register: " + args[0]};
    }
    
    if (logger->addRegister(args[0], regInfo->id)) {
        return {true, "Added register '" + args[0] + "' to logging"};
    }
    return {false, "Register '" + args[0] + "' is already being logged"};
}

CommandHandler::CommandResult CommandHandler::handleLogRemove(const std::vector<std::string>& args) {
    if (!logger) {
        return {false, "Logging is not enabled"};
    }
    if (args.size() != 1) {
        return {false, "Usage: log-remove <register_name>"};
    }
    
    if (logger->removeRegister(args[0])) {
        return {true, "Removed register '" + args[0] + "' from logging"};
    }
    return {false, "Register '" + args[0] + "' was not being logged"};
}

CommandHandler::CommandResult CommandHandler::handleLogStatus(const std::vector<std::string>& args) {
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