// FrameInterpreter.h
#ifndef FRAME_INTERPRETER_H
#define FRAME_INTERPRETER_H

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include "StMpcDefinitions.h"

class FrameInterpreter {
public:
    struct ResponseInfo {
        bool isSuccess;
        uint8_t payloadLength;
        std::vector<uint8_t> payload;
        bool validCRC;
    };

    std::string interpretResponse(const std::vector<uint8_t>& response);
    void printResponse(const std::vector<uint8_t>& response);

private:
    ResponseInfo parseResponse(const std::vector<uint8_t>& response);
    std::string interpretSuccessResponse(const ResponseInfo& info);
    std::string interpretErrorResponse(const ResponseInfo& info);
    bool validateCRC(const std::vector<uint8_t>& frame);
    std::string formatValue(const std::vector<uint8_t>& payload);
    static std::string byteToHex(uint8_t byte);

    const std::unordered_map<uint8_t, std::string> errorCodes = {
        {0x01, "Invalid Frame ID"},
        {0x02, "Register is Read-Only"},
        {0x03, "Register is Write-Only"},
        {0x04, "No Target Motor Selected"},
        {0x05, "Value Out of Range"},
        {0x0A, "CRC Error"}
    };
};

#endif // FRAME_INTERPRETER_H