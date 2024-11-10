// FrameInterpreter.h
#ifndef FRAME_INTERPRETER_H
#define FRAME_INTERPRETER_H

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "StMpcDefinitions.h"

class FrameInterpreter 
{
public:
    struct ResponseInfo 
    {
        bool isSuccess;
        uint8_t payloadLength;
        std::vector<uint8_t> payload;
        bool validCRC;
    };
    std::string interpretResponse(const std::vector<uint8_t>& response);
    std::string interpretResponse(const std::vector<uint8_t>& response, ST_MPC::RegisterType type);
    void printResponse(const std::vector<uint8_t>& response);

private:
    ResponseInfo parseResponse(const std::vector<uint8_t>& response);
    std::string interpretSuccessResponse(const ResponseInfo& info);
    std::string interpretSuccessResponse(const ResponseInfo& info, ST_MPC::RegisterType type);
    std::string interpretErrorResponse(const ResponseInfo& info);
    bool validateCRC(const std::vector<uint8_t>& frame);
    std::string formatValue(const std::vector<uint8_t>& payload);
    std::string formatValue(const std::vector<uint8_t>& payload, ST_MPC::RegisterType type);
    static std::string byteToHex(uint8_t byte);

    const std::unordered_map<ST_MPC::AckErrorId, std::string> errorCodes = 
    {
        {ST_MPC::AckErrorId::FrameId, "Invalid Frame ID"},
        {ST_MPC::AckErrorId::SetReadOnly, "Register is Read-Only"},
        {ST_MPC::AckErrorId::GetWriteOnly, "Register is Write-Only"},
        {ST_MPC::AckErrorId::NoTargetMotor, "No Target Motor Selected"},
        {ST_MPC::AckErrorId::OutOfRange, "Value Out of Range"},
        {ST_MPC::AckErrorId::BadCrc, "CRC Error"}
    };
};

#endif // FRAME_INTERPRETER_H