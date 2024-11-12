#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include "Logger.h"

CommandHandler::CommandHandler(SerialConnection& conn) : connection(conn) 
{
    // Initialize command map
    commandMap = 
    {
        {"set",     [this](const std::string& args) { return handleSetRegister(args); }},
        {"get",     [this](const std::string& args) { return handleGetRegister(args); }},
        {"exec",    [this](const std::string& args) { return handleExecute(args); }},
        {"ramp",    [this](const std::string& args) { return handleRamp(args); }},
        {"current", [this](const std::string& args) { return handleCurrentRef(args); }} 
    };

    // Initialize register map with type information and descriptions
    registerMap = 
    {
        {"motor-id", {ST_MPC::RegisterId::TargetMotor, ST_MPC::RegisterType::UInt8}},
        {"flags", {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32}},
        {"status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8}},
        {"control-mode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8}},
        {"speed-ref", {ST_MPC::RegisterId::SpeedRef, ST_MPC::RegisterType::Int32}},
        {"speed-Kp", {ST_MPC::RegisterId::SpeedKp, ST_MPC::RegisterType::UInt16}},
        {"speed-Ki", {ST_MPC::RegisterId::SpeedKi, ST_MPC::RegisterType::UInt16}},
        {"speed-Kd", {ST_MPC::RegisterId::SpeedKd, ST_MPC::RegisterType::UInt16}},
        {"torque-ref", {ST_MPC::RegisterId::TorqueRef, ST_MPC::RegisterType::Int16}},
        {"torque-Kp", {ST_MPC::RegisterId::TorqueKp, ST_MPC::RegisterType::UInt16}},
        {"torque-Ki", {ST_MPC::RegisterId::TorqueKi, ST_MPC::RegisterType::UInt16}},
        {"torque-Kd", {ST_MPC::RegisterId::TorqueKd, ST_MPC::RegisterType::UInt16}},
        {"flux-ref", {ST_MPC::RegisterId::FluxRef, ST_MPC::RegisterType::Int16}},
        {"flux-Kp", {ST_MPC::RegisterId::FluxKp, ST_MPC::RegisterType::UInt16}},
        {"flux-Ki", {ST_MPC::RegisterId::FluxKi, ST_MPC::RegisterType::UInt16}},
        {"flux-Kd", {ST_MPC::RegisterId::FluxKd, ST_MPC::RegisterType::UInt16}},
        {"status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8}},
        {"flags",  {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32}},
        {"control-mode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8}},
        {"motor-power", {ST_MPC::RegisterId::MotorPower, ST_MPC::RegisterType::UInt16}},
        {"speed-meas", {ST_MPC::RegisterId::SpeedMeas, ST_MPC::RegisterType::Int32}},
        {"torque-meas", {ST_MPC::RegisterId::TorqueMeas, ST_MPC::RegisterType::Int16}},
        {"flux-meas", {ST_MPC::RegisterId::FluxMeas, ST_MPC::RegisterType::Int16}},
        {"Ia", {ST_MPC::RegisterId::Ia, ST_MPC::RegisterType::Int16}},
        {"Ib", {ST_MPC::RegisterId::Ib, ST_MPC::RegisterType::Int16}},
        {"Ialpha", {ST_MPC::RegisterId::Ialpha, ST_MPC::RegisterType::Int16}},
        {"Ibeta", {ST_MPC::RegisterId::Ibeta, ST_MPC::RegisterType::Int16}},
        {"Iq", {ST_MPC::RegisterId::Iq, ST_MPC::RegisterType::Int16}},
        {"Id", {ST_MPC::RegisterId::Id, ST_MPC::RegisterType::Int16}},
        {"Iq-ref", {ST_MPC::RegisterId::IqRef, ST_MPC::RegisterType::Int16}},
        {"Id-ref", {ST_MPC::RegisterId::IdRef, ST_MPC::RegisterType::Int16}},
        {"Vq", {ST_MPC::RegisterId::Vq, ST_MPC::RegisterType::Int16}},
        {"Vd", {ST_MPC::RegisterId::Vd, ST_MPC::RegisterType::Int16}},
        {"Valpha", {ST_MPC::RegisterId::Valpha, ST_MPC::RegisterType::Int16}},
        {"Vbeta", {ST_MPC::RegisterId::Vbeta, ST_MPC::RegisterType::Int16}},
        {"el-angle-meas", {ST_MPC::RegisterId::ElAngleMeas, ST_MPC::RegisterType::Int16}},
        {"ramp-final-speed", {ST_MPC::RegisterId::RampFinalSpeed, ST_MPC::RegisterType::Int32}},
        {"ramp-duration", {ST_MPC::RegisterId::RampDuration, ST_MPC::RegisterType::UInt16}},
        {"speed-Kp-div", {ST_MPC::RegisterId::SpeedKpDiv, ST_MPC::RegisterType::UInt16}},
        {"speed-Ki-div", {ST_MPC::RegisterId::SpeedKiDiv, ST_MPC::RegisterType::UInt16}},
        {"trans-det-1000", {ST_MPC::RegisterId::TransDetReg1000, ST_MPC::RegisterType::UInt8}},
        {"trans-det-1200", {ST_MPC::RegisterId::TransDetReg1200, ST_MPC::RegisterType::UInt8}},
        {"trans-det-1300", {ST_MPC::RegisterId::TransDetReg1300, ST_MPC::RegisterType::UInt8}},
        {"trans-det-Id", {ST_MPC::RegisterId::TransDetRegId, ST_MPC::RegisterType::UInt8}},
        {"dead-time-Id", {ST_MPC::RegisterId::DeadTimeRegId, ST_MPC::RegisterType::UInt8}},
        {"dead-time-A", {ST_MPC::RegisterId::DeadTimeRegA, ST_MPC::RegisterType::UInt8}},
        {"dead-time-B", {ST_MPC::RegisterId::DeadTimeRegB, ST_MPC::RegisterType::UInt8}},
        {"gdr-pwr-dis", {ST_MPC::RegisterId::GdrPwrDis, ST_MPC::RegisterType::UInt8}},
        {"gdr-pwm-en", {ST_MPC::RegisterId::GdrPwmEn, ST_MPC::RegisterType::UInt8}},
        {"gdr-flt-A", {ST_MPC::RegisterId::GdrFltPhA, ST_MPC::RegisterType::UInt8}},
        {"gdr-flt-B", {ST_MPC::RegisterId::GdrFltPhB, ST_MPC::RegisterType::UInt8}},
        {"gdr-flt-C", {ST_MPC::RegisterId::GdrFltPhC, ST_MPC::RegisterType::UInt8}},
        {"gdr-temp-A", {ST_MPC::RegisterId::GdrTempPhA, ST_MPC::RegisterType::UInt32}},
        {"gdr-temp-B", {ST_MPC::RegisterId::GdrTempPhB, ST_MPC::RegisterType::UInt32}},
        {"gdr-temp-C", {ST_MPC::RegisterId::GdrTempPhC, ST_MPC::RegisterType::UInt32}},
        {"mux-Id", {ST_MPC::RegisterId::MuxRegId, ST_MPC::RegisterType::UInt8}},
        {"torque-Kp-div-pow2", {ST_MPC::RegisterId::TorqueKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"torque-Ki-div-pow2", {ST_MPC::RegisterId::TorqueKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"flux-Kp-div-pow2", {ST_MPC::RegisterId::FluxKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"flux-Ki-div-pow2", {ST_MPC::RegisterId::FluxKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"speed-Kp-div-pow2", {ST_MPC::RegisterId::SpeedKpDivPow2, ST_MPC::RegisterType::UInt16}},
        {"speed-Ki-div-pow2", {ST_MPC::RegisterId::SpeedKiDivPow2, ST_MPC::RegisterType::UInt16}},
        {"torque-Kp-div", {ST_MPC::RegisterId::TorqueKpDiv, ST_MPC::RegisterType::UInt16}},
        {"torque-Ki-div", {ST_MPC::RegisterId::TorqueKiDiv, ST_MPC::RegisterType::UInt16}},
        {"flux-Kp-div", {ST_MPC::RegisterId::FluxKpDiv, ST_MPC::RegisterType::UInt16}},
        {"flux-Ki-div", {ST_MPC::RegisterId::FluxKiDiv, ST_MPC::RegisterType::UInt16}},
        {"align-final-flux", {ST_MPC::RegisterId::AlignFinalFlux, ST_MPC::RegisterType::UInt16}},
        {"align-ramp-up-duration", {ST_MPC::RegisterId::AlignRampUpDuration, ST_MPC::RegisterType::UInt16}},
        {"align-ramp-down-duration", {ST_MPC::RegisterId::AlignRampDownDuration, ST_MPC::RegisterType::UInt16}},
        {"is-aligned", {ST_MPC::RegisterId::IsAligned, ST_MPC::RegisterType::UInt16}},
        {"git-version", {ST_MPC::RegisterId::GitVersion, ST_MPC::RegisterType::CharPtr}}

    };

    // Initialize execute command map
    executeMap = 
    {
        {"start", ST_MPC::ExecuteId::StartMotor},
        {"stop", ST_MPC::ExecuteId::StopMotor},
        {"align", ST_MPC::ExecuteId::EncoderAlign},
        {"start-stop", ST_MPC::ExecuteId::StartStop},
        {"stop-ramp", ST_MPC::ExecuteId::StopRamp},
        {"reset", ST_MPC::ExecuteId::Reset},
        {"ping", ST_MPC::ExecuteId::Ping},
        {"fault-ack", ST_MPC::ExecuteId::FaultAck}
    };

    statusMap = 
    {
        {"idle", ST_MPC::Status::Idle},
        {"idle-alignment", ST_MPC::Status::IdleAlignment},
        {"alignment", ST_MPC::Status::Alignment},
        {"idle-start", ST_MPC::Status::IdleStart},
        {"start", ST_MPC::Status::Start},
        {"start-run", ST_MPC::Status::StartRun},
        {"run", ST_MPC::Status::Run},
        {"stop", ST_MPC::Status::Stop},
        {"stop-idle", ST_MPC::Status::StopIdle},
        {"fault-now", ST_MPC::Status::FaultNow},
        {"fault-over", ST_MPC::Status::FaultOver}
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
    commandMap["log-config"] = [this](const std::string& args) { return handleLogConfig(args); };
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

const std::string CommandHandler::printAllRegisters() const
{
    std::stringstream ss;
    for (const auto& reg : registerMap) {
        ss << std::left << "\t" << std::setw(25) << reg.first 
            << " - (0x" << std::right
            << std::hex << std::setw(2) << std::setfill('0') 
            << static_cast<unsigned int>(reg.second.id) << ")"
            << std::setfill(' ') << std::dec << " - ";
        switch (reg.second.type) {
            case ST_MPC::RegisterType::UInt8: 
                ss << "UInt8"; 
                break;
            case ST_MPC::RegisterType::UInt16: 
                ss << "UInt16"; 
                break;
            case ST_MPC::RegisterType::UInt32: 
                ss << "UInt32"; 
                break;
            case ST_MPC::RegisterType::Int16: 
                ss << "Int16"; 
                break;
            case ST_MPC::RegisterType::Int32: 
                ss << "Int32"; 
                break;
            case ST_MPC::RegisterType::CharPtr: 
                ss << "CharPtr"; 
                break;
            default: 
                ss << "Unknown"; 
                break;
        }
        ss << "\n";
    }
    return ss.str();
}

const std::string CommandHandler::printAllExecutes() const
{
    std::stringstream ss;
    for (const auto& exec : executeMap) {
        ss << "\t" << exec.first << "\n";
    }
    return ss.str();
}

const std::string CommandHandler::printAllStatuses() const
{
    std::stringstream ss;
    for (const auto& status : statusMap) {
        ss << "\t" 
           << std::left << std::setw(25) << status.first
           << " - (0x" 
           << std::right << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<unsigned int>(status.second) 
           << ")" << std::dec << std::setfill(' ')
           << "\n";
    }
    return ss.str();
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
        auto frame = frameBuilder.buildSetFrame(1, reg.id, regVal, reg.type);
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
        auto frame = frameBuilder.buildGetFrame(1, reg.id);
        auto response = sendAndProcessResponse(frame, reg.type);
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
    int32_t duration;

    if (!(iss >> finalSpeed >> duration)) {
        return {false, "Usage: ramp <final_speed> <duration>"};
    }

    if (duration <= 0) {
        return {false, "Duration must be greater than 0"};
    }

    try {
        auto frame = frameBuilder.buildRampFrame(1, finalSpeed, static_cast<uint16_t>(duration));
        auto response = sendAndProcessResponse(frame);
        return {true, "Started ramp: " + std::to_string(finalSpeed) + " rpm over " 
                    + std::to_string(duration) + " ms\n" + response};
    } catch (const std::exception& e) {
        return handleError("handleRamp failed", e);
    }
}

CommandHandler::CommandResult CommandHandler::handleCurrentRef(const std::string& args)
{
    std::istringstream iss(args);
    int16_t IqRef;
    int16_t IdRef;

    if (!(iss >> IqRef >> IdRef)) {
        return {false, "Usage: current-ref <Iq> <Id>"};
    }

    try {
        auto frame = frameBuilder.buildCurrentFrame(1, IqRef, IdRef);
        auto response = sendAndProcessResponse(frame);
        return {true, "Set current references to Iq: " + std::to_string(IqRef) + ", Id: " 
                + std::to_string(IdRef) + "\n" + response};
    } catch (const std::exception& e) {
        return handleError("handleCurrentRef failed", e);
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
    startPlot();
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
    stopPlot();
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
    Logger::LogConfig logConfig = logger->getConfig();
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
    status += "Config:\n";
    status += "  Filename: " + logConfig.filename + "\n";
    status += "  Sample interval: " + std::to_string(logConfig.sampleInterval.count()) + " ms\n";
    
    return {true, status};
}

CommandHandler::CommandResult CommandHandler::handleLogConfig(const std::string& args)
{
    if (!logger) {
        return {false, "Logging is not enabled"};
    }

    std::istringstream iss(args);
    std::string filename;
    int sampleInterval;

    if (!(iss >> filename >> sampleInterval)) {
        return {false, "Usage: log-config <filename> <sample_interval_ms>"};
    }

    if (sampleInterval <= 0) {
        return {false, "Sample interval must be positive"};
    }

    // Start with current config to preserve other settings
    Logger::LogConfig config = logger->getConfig();
    config.filename = filename;
    config.sampleInterval = std::chrono::milliseconds(sampleInterval);
    
    logger->setConfig(config);

    return {true, "Logging configuration updated"};
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

std::string CommandHandler::sendAndProcessResponse(const std::vector<uint8_t>& frame, ST_MPC::RegisterType type)
{
    connection.sendFrame(frame);
    auto response = connection.readFrame();
    frameInterpreter.printResponse(response);
    return frameInterpreter.interpretResponse(response, type);

}

void CommandHandler::startPlot()
{
    pid_t pid = fork();
    if (pid == 0) {
        Logger::LogConfig config = logger->getConfig();
        execlp("python3", "python3", "plot.py", config.filename.c_str(), nullptr);
        std::cerr << "Failed to start plotter process" << std::endl;
    } else if (pid > 0) {
        std::cout << "Plotter process started with PID " << pid << std::endl;
        plotterPid = pid;
    } else {
        std::cerr << "Failed to fork plotter process" << std::endl;
    }
}

void CommandHandler::stopPlot()
{
    if (plotterPid > 0) {
        kill(plotterPid, SIGTERM);
        plotterPid = 0;
    }
}