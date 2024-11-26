#include "CommandHandlerRt.h"
#include <sstream>
#include <iomanip>

CommandHandlerRt::CommandHandlerRt(SerialConnectionRt& conn, uint8_t mscId) 
    : connection(conn), mscId(mscId)
{
    // Initialize command map
    commandMap = {
        {"read", std::bind(&CommandHandlerRt::handleRead, this, std::placeholders::_1)},
        {"write", std::bind(&CommandHandlerRt::handleWrite, this, std::placeholders::_1)},
        {"exec", std::bind(&CommandHandlerRt::handleExecute, this, std::placeholders::_1)},
        {"foc-read", std::bind(&CommandHandlerRt::handleFocRead, this, std::placeholders::_1)},
        {"foc-write", std::bind(&CommandHandlerRt::handleFocWrite, this, std::placeholders::_1)},
        {"foc-exec", std::bind(&CommandHandlerRt::handleFocExecute, this, std::placeholders::_1)},
        {"log-start", std::bind(&CommandHandlerRt::handleLogStart, this, std::placeholders::_1)},
        {"log-stop", std::bind(&CommandHandlerRt::handleLogStop, this, std::placeholders::_1)},
        {"log-add-rt", std::bind(&CommandHandlerRt::handleLogAddRt, this, std::placeholders::_1)},
        {"log-remove-rt", std::bind(&CommandHandlerRt::handleLogRemoveRt, this, std::placeholders::_1)},
        {"log-add-foc", std::bind(&CommandHandlerRt::handleLogAddFoc, this, std::placeholders::_1)},
        {"log-remove-foc", std::bind(&CommandHandlerRt::handleLogRemoveFoc, this, std::placeholders::_1)},
        {"log-status", std::bind(&CommandHandlerRt::handleLogStatus, this, std::placeholders::_1)},
        {"log-config", std::bind(&CommandHandlerRt::handleLogConfig, this, std::placeholders::_1)}
    };

    // Initialize register map
    registerMap = {
        {"rt-ramp-final-speed", {RT::RegisterId::RAMP_FINAL_SPEED, RT::RegisterType::Int32}},
        {"rt-ramp-duration", {RT::RegisterId::RAMP_DURATION, RT::RegisterType::UInt16}},
        {"rt-speed-ref", {RT::RegisterId::SPEED_SETPOINT, RT::RegisterType::Float}},
        {"rt-speed-Kp", {RT::RegisterId::SPEED_KP, RT::RegisterType::Float}},
        {"rt-speed-Ki", {RT::RegisterId::SPEED_KI, RT::RegisterType::Float}},
        {"rt-speed-Kd", {RT::RegisterId::SPEED_KD, RT::RegisterType::Float}},
        {"rt-board-info", {RT::RegisterId::BOARD_INFO, RT::RegisterType::CharPtr}},
        {"rt-speed-meas", {RT::RegisterId::CURRENT_SPEED, RT::RegisterType::Float}},
        {"rt-speed-loop-period", {RT::RegisterId::SPEED_LOOP_PERIOD_MS, RT::RegisterType::UInt32}},
        {"rt-git-version", {RT::RegisterId::GIT_VERSION, RT::RegisterType::CharPtr}}
    };

    // Initialize execute map
    executeMap = {
        {"start", RT::ExecuteId::START_MOTOR},
        {"stop", RT::ExecuteId::STOP_MOTOR},
        {"ramp", RT::ExecuteId::RAMP_EXECUTE},
        {"feedback-start", RT::ExecuteId::START_FEEDBACK},
        {"feedback-stop", RT::ExecuteId::STOP_FEEDBACK}
    };

    // Initialize FOC register map
    focRegisterMap = {
        {"motor-id", {ST_MPC::RegisterId::TargetMotor, ST_MPC::RegisterType::UInt8}},
        {"flags", {ST_MPC::RegisterId::Flags, ST_MPC::RegisterType::UInt32}},
        {"status", {ST_MPC::RegisterId::Status, ST_MPC::RegisterType::UInt8}},
        {"control-mode", {ST_MPC::RegisterId::ControlMode, ST_MPC::RegisterType::UInt8}},
        {"speed-ref", {ST_MPC::RegisterId::SpeedRef, ST_MPC::RegisterType::Int32}},
        {"speed-Kp", {ST_MPC::RegisterId::SpeedKp, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"speed-Ki", {ST_MPC::RegisterId::SpeedKi, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"speed-Kd", {ST_MPC::RegisterId::SpeedKd, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"torque-ref", {ST_MPC::RegisterId::TorqueRef, ST_MPC::RegisterType::Int16}},
        {"torque-Kp", {ST_MPC::RegisterId::TorqueKp, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"torque-Ki", {ST_MPC::RegisterId::TorqueKi, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"torque-Kd", {ST_MPC::RegisterId::TorqueKd, ST_MPC::RegisterType::Int16}}, // bug in SDK: is Int16
        {"flux-ref", {ST_MPC::RegisterId::FluxRef, ST_MPC::RegisterType::Int16}},
        {"flux-Kp", {ST_MPC::RegisterId::FluxKp, ST_MPC::RegisterType::Int16}},     // bug in SDK: is Int16
        {"flux-Ki", {ST_MPC::RegisterId::FluxKi, ST_MPC::RegisterType::Int16}},     // bug in SDK: is Int16
        {"flux-Kd", {ST_MPC::RegisterId::FluxKd, ST_MPC::RegisterType::Int16}},     // bug in SDK: is Int16
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
        {"Iq-ref-speed-mode", {ST_MPC::RegisterId::IqRefSpeedMode, ST_MPC::RegisterType::Int16}},
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
        {"align-ramp-up", {ST_MPC::RegisterId::AlignRampUpDuration, ST_MPC::RegisterType::UInt16}},
        {"align-ramp-down", {ST_MPC::RegisterId::AlignRampDownDuration, ST_MPC::RegisterType::UInt16}},
        {"is-aligned", {ST_MPC::RegisterId::IsAligned, ST_MPC::RegisterType::UInt16}},
        {"git-version", {ST_MPC::RegisterId::GitVersion, ST_MPC::RegisterType::CharPtr}}
    };

    focExecuteMap = {   
        {"start", {ST_MPC::ExecuteId::StartMotor}}, 
        {"stop", {ST_MPC::ExecuteId::StopMotor}}, 
        {"stop-ramp", {ST_MPC::ExecuteId::StopRamp}},
        {"reset", {ST_MPC::ExecuteId::Reset}},
        {"ping", {ST_MPC::ExecuteId::Ping}},
        {"encoder-align", {ST_MPC::ExecuteId::EncoderAlign}},
        {"start-stop", {ST_MPC::ExecuteId::StartStop}},
        {"fault-ack", {ST_MPC::ExecuteId::FaultAck}} 
    };
    
}

CommandHandlerRt::CommandHandlerRt(SerialConnectionRt& conn, uint8_t mscId, LoggerRt& logger)
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
    std::string cmd;
    iss >> cmd;

    try {
        if (cmd == "ramp") {
            // Parse ramp parameters
            int32_t finalSpeed;
            uint16_t duration;
            iss >> finalSpeed >> duration;
            
            if (iss.fail()) {
                return {false, "Usage: exec ramp <final-speed> <duration>"};
            }

            // 1. Set ramp final speed
            auto speedFrame = frameBuilder.buildWriteFrame(mscId, RT::RegisterId::RAMP_FINAL_SPEED, 
                                                         finalSpeed, RT::RegisterType::Float);
            auto speedResponse = sendAndProcessResponse(speedFrame);
            
            // 2. Set ramp duration
            auto durationFrame = frameBuilder.buildWriteFrame(mscId, RT::RegisterId::RAMP_DURATION, 
                                                            duration, RT::RegisterType::UInt16);
            auto durationResponse = sendAndProcessResponse(durationFrame);
            
            // 3. Execute ramp command
            auto execFrame = frameBuilder.buildExecuteFrame(mscId, RT::ExecuteId::RAMP_EXECUTE);
            auto execResponse = sendAndProcessResponse(execFrame);

            if (execResponse.find("Error") != std::string::npos) {
                return {false, execResponse};
            }
            return {true, "Ramp command executed successfully"};
        }
        else {
            // Handle other execute commands (start, stop, etc.)
            auto it = executeMap.find(cmd);
            if (it == executeMap.end()) {
                return {false, "Unknown execute command: " + cmd};
            }

            auto frame = frameBuilder.buildExecuteFrame(mscId, it->second);
            std::string response = sendAndProcessResponse(frame);
            return {true, response};
        }
    }
    catch (const std::exception& e) {
        return handleError("Execute failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleFoc(const std::string& args) 
{
    std::istringstream iss(args);
    std::string cmdType;
    iss >> cmdType;

    if (cmdType == "foc-read") {
        std::cout << "CommandHandlerRt::handleFoc() - foc-read \n\targs: " << args << std::endl;
        return handleFocRead(args);
    } 
    else if (cmdType == "foc-write") {
        std::cout << "CommandHandlerRt::handleFoc() - foc-write \n\targs: " << args << std::endl;
        return handleFocWrite(args);
    }
    else if (cmdType == "foc-exec") {
        std::cout << "CommandHandlerRt::handleFoc() - foc-exec \n\targs: " << args << std::endl;
        return handleFocExecute(args);
    }
    return {false, "Unknown FOC command type: " + cmdType};
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleFocRead(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName;
    iss >> regName;
    auto it = focRegisterMap.find(regName);
    if (it == focRegisterMap.end()) {
        return {false, "Unknown FOC register: " + regName};
    }

    try {
        const auto& reg = it->second;
        auto frame = frameBuilder.buildFocReadFrame(mscId, reg.id);
        std::string response = sendAndProcessResponse(frame, reg.type);
        if (regName == "gdr-temp-A" || regName == "gdr-temp-B" || regName == "gdr-temp-C") {
            std::size_t start = response.find('=') + 1;
            std::size_t end = response.find('(');
            std::string value = response.substr(start, end - start);
            float temp = std::stof(value) * 5.42f / 100.0f - 244.0f;
            return {true, response + " (" + std::to_string(temp) + " C)"};
        } else if (regName == "bus-voltage") {
            std::size_t start = response.find('=') + 1;
            std::size_t end = response.find('(');
            std::string value = response.substr(start, end - start);
            float voltage = std::stof(value) * 5.0f / 0.001635f / 65536.0f;
            return {true, response + " (" + std::to_string(voltage) + " V)"};
        } else if (regName == "control-mode") {
            std::size_t start = response.find('=') + 1;
            std::size_t end = response.find('(');
            std::string value = response.substr(start, end - start);
            int mode = std::stoi(value);
            if (mode == 0) {
                return {true, response + " (Torque)"};
            } else if (mode == 1) {
                return {true, response + " (Speed)"};
            } else {
                return {true, response + " (Unknown)"};
            }
        } else {
            return {true, response};
        }
        
    }
    catch (const std::exception& e) {
        return handleError("FOC read failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleFocWrite(const std::string& args) 
{
    std::istringstream iss(args);
    std::string regName, valueStr;
    iss >> regName >> valueStr;  // Skip "foc-write" command

    auto it = focRegisterMap.find(regName);
    if (it == focRegisterMap.end()) {
        return {false, "Unknown FOC register: " + regName};
    }

    try {
        const auto& reg = it->second;
        int32_t value;
        std::istringstream(valueStr) >> value;

        auto frame = frameBuilder.buildFocWriteFrame(mscId, reg.id, value, reg.type);
        std::string response = sendAndProcessResponse(frame, reg.type);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("FOC write failed", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleFocExecute(const std::string& args) 
{
    std::istringstream iss(args);
    std::string execName;
    iss >> execName; 
    auto it = focExecuteMap.find(execName);
    if (it == focExecuteMap.end()) {
        return {false, "Unknown FOC execute command: " + execName};
    }
    try {
        auto frame = frameBuilder.buildFocExecuteFrame(mscId, it->second);
        std::string response = sendAndProcessResponse(frame);
        return {true, response};
    }
    catch (const std::exception& e) {
        return handleError("FOC execute failed", e);
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
        startPlot();
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
        stopPlot();
        logger->stop();
        return {true, "Logging stopped"};
    }
    catch (const std::exception& e) {
        return handleError("Failed to stop logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogAddRt(const std::string& args) 
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
        if (logger->addRtRegister(regName, reg.id, reg.type)) {
            return {true, "Register added to logging: " + regName};
        }
        return {false, "Register already being logged: " + regName};
    }
    catch (const std::exception& e) {
        return handleError("Failed to add register to logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogRemoveRt(const std::string& args) 
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

    if (logger->removeRtRegister(regName)) {
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

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogAddFoc(const std::string& args)
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
        const auto& reg = focRegisterMap.find(regName);
        if (reg == focRegisterMap.end()) {
            return {false, "Unknown FOC register: " + regName};
        }

        if (logger->addFocRegister(regName, reg->second.id, reg->second.type)) {
            return {true, "FOC register added to logging: " + regName};
        }
        return {false, "FOC register already being logged: " + regName};
    }
    catch (const std::exception& e) {
        return handleError("Failed to add FOC register to logging", e);
    }
}

CommandHandlerRt::CommandResult CommandHandlerRt::handleLogRemoveFoc(const std::string& args)
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

    if (logger->removeFocRegister(regName)) {
        return {true, "FOC register removed from logging: " + regName};
    }
    return {false, "FOC register not found in logging: " + regName};
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

std::string CommandHandlerRt::sendAndProcessResponse(const std::vector<uint8_t>& frame, 
                                                   ST_MPC::RegisterType type) 
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

const std::string CommandHandlerRt::printAllFocRegisters() const
{
    std::stringstream ss;
    ss << "Available registers:\n";
    for (const auto& reg : focRegisterMap) {
        ss << "  " << std::setw(20) << std::left << reg.first;
        switch (reg.second.type) {
            case ST_MPC::RegisterType::UInt8: ss << "UINT8"; break;
            case ST_MPC::RegisterType::Int16: ss << "INT16"; break;
            case ST_MPC::RegisterType::UInt16: ss << "UINT16"; break;
            case ST_MPC::RegisterType::Int32: ss << "INT32"; break;
            case ST_MPC::RegisterType::UInt32: ss << "UINT32"; break;
            case ST_MPC::RegisterType::CharPtr: ss << "STRING"; break;
        }
        ss << "\n";
    }
    return ss.str();
}

const std::string CommandHandlerRt::printAllFocExecutes() const
{
    std::stringstream ss;
    ss << "Available FOC execute commands:\n";
    for (const auto& exec : focExecuteMap) {
        ss << "  " << exec.first << "\n";
    }
    return ss.str();
}

void CommandHandlerRt::startPlot()
{
    pid_t pid = fork();
    if (pid == 0) {
        LoggerRt::LogConfig config = logger->getConfig();
        execlp("python3", "python3", "plot.py", config.filename.c_str(), nullptr);
        std::cerr << "Failed to start plotter process" << std::endl;
    } else if (pid > 0) {
        std::cout << "Plotter process started with PID " << pid << std::endl;
        plotterPid = pid;
    } else {
        std::cerr << "Failed to fork plotter process" << std::endl;
    }
}

void CommandHandlerRt::stopPlot()
{
    if (plotterPid > 0) {
        kill(plotterPid, SIGTERM);
        plotterPid = 0;
    }
}