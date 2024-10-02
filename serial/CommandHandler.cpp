#include "CommandHandler.h"
#include <iostream>
#include <sstream>
#include <unordered_map>

constexpr uint8_t predefinedMotorID = 1;

// Map string names to ST_MPC::RegisterId
std::unordered_map<std::string, ST_MPC::RegisterId> registerIdMap = {
    {"SpeedKp", ST_MPC::RegisterId::SpeedKp},
    {"SpeedKi", ST_MPC::RegisterId::SpeedKi},
    {"SpeedKd", ST_MPC::RegisterId::SpeedKd},
    {"SpeedRef", ST_MPC::RegisterId::SpeedRef},
    {"TorqueKp", ST_MPC::RegisterId::TorqueKp},
    {"TorqueKi", ST_MPC::RegisterId::TorqueKi},
    {"TorqueKd", ST_MPC::RegisterId::TorqueKd},
    {"TorqueRef", ST_MPC::RegisterId::TorqueRef},
    {"FluxKp", ST_MPC::RegisterId::FluxKp},
    {"FluxKi", ST_MPC::RegisterId::FluxKi},
    {"FluxKd", ST_MPC::RegisterId::FluxKd},
    {"FluxRef", ST_MPC::RegisterId::FluxRef},
    {"MotorPower", ST_MPC::RegisterId::MotorPower},
    {"SpeedMeas", ST_MPC::RegisterId::SpeedMeas},
    {"TorqueMeas", ST_MPC::RegisterId::TorqueMeas},
    {"FluxMeas", ST_MPC::RegisterId::FluxMeas},
    {"RampFinalSpeed", ST_MPC::RegisterId::RampFinalSpeed},
    {"RampDuration", ST_MPC::RegisterId::RampDuration}
};

// Map string names to ST_MPC::ExecuteId
std::unordered_map<std::string, ST_MPC::ExecuteId> executeIdMap = {
    {"StartMotor", ST_MPC::ExecuteId::StartMotor},
    {"StopMotor", ST_MPC::ExecuteId::StopMotor},
    {"EncoderAlign", ST_MPC::ExecuteId::EncoderAlign},
    {"StartStop", ST_MPC::ExecuteId::StartStop},
    {"StopRamp", ST_MPC::ExecuteId::StopRamp},
    {"Reset", ST_MPC::ExecuteId::Reset},
    {"Ping", ST_MPC::ExecuteId::Ping},
    {"FaultAck", ST_MPC::ExecuteId::FaultAck}
};


CommandHandler::CommandHandler(SerialConnection& conn)
    : connection(conn) {}

void CommandHandler::processUserCommand(const std::string& command) 
{
    std::stringstream ss(command);
    std::string cmd;
    ss >> cmd;

    if (cmd == "set-register") {
        std::string regName;
        int regVal;

        ss >> regName >> regVal;

        // Check if the register name exists in the map
        if (registerIdMap.find(regName) != registerIdMap.end()) {
            ST_MPC::RegisterId regID = registerIdMap[regName];
            std::vector<uint8_t> regValBytes = {
                static_cast<uint8_t>(regVal & 0xFF),
                static_cast<uint8_t>((regVal >> 8) & 0xFF)
            };

            auto frame = frameBuilder.buildSetRegisterFrame(predefinedMotorID, regID, regValBytes);
            connection.sendFrame(frame);

            auto response = connection.readFrame();
            frameInterpreter.printResponse(response);
            std::cout << frameInterpreter.interpretResponse(response) << std::endl;
        } else {
            std::cerr << "Unknown register name: " << regName << std::endl;
        }
    } 
    else if (cmd == "get-register") {
        std::string regName;

        ss >> regName;

        // Check if the register name exists in the map
        if (registerIdMap.find(regName) != registerIdMap.end()) {
            ST_MPC::RegisterId regID = registerIdMap[regName];

            auto frame = frameBuilder.buildGetRegisterFrame(predefinedMotorID, regID);
            connection.sendFrame(frame);

            auto response = connection.readFrame();
            frameInterpreter.printResponse(response);
            std::cout << frameInterpreter.interpretResponse(response) << std::endl;
        } else {
            std::cerr << "Unknown register name: " << regName << std::endl;
        }
    } 
    else if (cmd == "execute") {
        std::string execName;

        ss >> execName;

        // Check if the execute name exists in the map
        if (executeIdMap.find(execName) != executeIdMap.end()) {
            ST_MPC::ExecuteId execID = executeIdMap[execName];

            auto frame = frameBuilder.buildExecuteFrame(predefinedMotorID, execID);
            connection.sendFrame(frame);

            auto response = connection.readFrame();
            frameInterpreter.printResponse(response);
            std::cout << frameInterpreter.interpretResponse(response) << std::endl;
        } else {
            std::cerr << "Unknown execute command: " << execName << std::endl;
        }
    } 
    else if (cmd == "executeRamp") {
        int32_t finalSpeed;
        uint16_t duration;

        ss >> finalSpeed >> duration;

        auto frame = frameBuilder.buildExecuteRampFrame(predefinedMotorID, finalSpeed, duration);
        connection.sendFrame(frame);

        auto response = connection.readFrame();
        frameInterpreter.printResponse(response);
        std::cout << frameInterpreter.interpretResponse(response) << std::endl;
    }
    else {
        std::cerr << "Unknown command." << std::endl;
    }
}
