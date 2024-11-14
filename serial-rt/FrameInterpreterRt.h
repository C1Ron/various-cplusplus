#ifndef FRAME_INTERPRETER_RT_H
#define FRAME_INTERPRETER_RT_H

#include "RtDefinitions.h"
#include <vector>
#include <string>
#include <unordered_map>

class FrameInterpreterRt 
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
    std::string interpretResponse(const std::vector<uint8_t>& response, RT::RegisterType type);
    void printResponse(const std::vector<uint8_t>& response);

private:
    ResponseInfo parseResponse(const std::vector<uint8_t>& response);
    std::string interpretSuccessResponse(const ResponseInfo& info);
    std::string interpretSuccessResponse(const ResponseInfo& info, RT::RegisterType type);
    std::string interpretErrorResponse(const ResponseInfo& info);
    bool validateCRC(const std::vector<uint8_t>& frame);
    std::string formatValue(const std::vector<uint8_t>& payload);
    std::string formatValue(const std::vector<uint8_t>& payload, RT::RegisterType type);
    static std::string byteToHex(uint8_t byte);

    const std::unordered_map<RT::ErrorId, std::string> errorCodes = {
        {RT::ErrorId::UNKNOWN_START_BYTE, "Unknown Start Byte"},
        {RT::ErrorId::BAD_CRC, "CRC Error"},
        {RT::ErrorId::NO_MSC, "No MSC"},
        {RT::ErrorId::MSC_NOT_PRESENT, "MSC Not Present"},
        {RT::ErrorId::MSC_TIMEOUT, "MSC Timeout"},
        {RT::ErrorId::INVALID_MSC, "Invalid MSC"},
        {RT::ErrorId::NO_ERROR, "No Error"}
    };
};

#endif // FRAME_INTERPRETER_RT_H