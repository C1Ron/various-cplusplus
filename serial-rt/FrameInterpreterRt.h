#ifndef FRAME_INTERPRETER_RT_H
#define FRAME_INTERPRETER_RT_H

#include "RtDefinitions.h"
#include "StMpcDefinitions.h"
#include <vector>
#include <string>
#include <unordered_map>

class FrameInterpreterRt 
{
public:
    // Main interpretation functions
    std::string interpretResponse(const std::vector<uint8_t>& response);
    std::string interpretResponse(const std::vector<uint8_t>& response, RT::RegisterType type);
    std::string interpretResponse(const std::vector<uint8_t>& response, ST_MPC::RegisterType type);
    void printResponse(const std::vector<uint8_t>& response);

private:
    struct Header 
    {
        // Field 0
        uint8_t startByte;
        uint8_t totalSize;
        uint8_t payloadSize;
        uint8_t mscId;
        
        // Field 1
        uint8_t msgRequestId;
        uint8_t msgResponseId;
        uint8_t conversationId;
        uint8_t senderId;
        
        // Field 2
        uint8_t numBlocks;
        uint8_t seqId;
        uint8_t commandType;
        uint8_t errorCode;
        
        // Field 3
        uint8_t futureUse0;
        uint8_t futureUse1;
        uint8_t futureUse2;
        uint8_t endByte;
    };

    struct ResponseInfo 
    {
        Header header;
        std::vector<uint8_t> payload;
        uint8_t crc;
        bool validCRC;
    };

    // Helper functions for different command types
    std::string handleRtRead(const ResponseInfo& info, RT::RegisterType type = RT::RegisterType::UInt8);
    std::string handleRtWrite(const ResponseInfo& info);
    std::string handleRtExecute(const ResponseInfo& info);
    std::string handleFocCommand(const ResponseInfo& info);
    
    // Parsing helpers
    ResponseInfo parseResponse(const std::vector<uint8_t>& response);
    Header parseHeader(const std::vector<uint8_t>& response);
    bool validateCRC(const std::vector<uint8_t>& frame) const;
    bool validateWriteReplyCRC(const std::vector<uint8_t>& frame) const;
    
    // Value formatting
    std::string formatValue(const std::vector<uint8_t>& payload, RT::RegisterType type) const;
    static std::string byteToHex(uint8_t byte);

    // Error handling
    const std::unordered_map<RT::ErrorId, std::string> errorCodes = {
        {RT::ErrorId::UNKNOWN_START_BYTE, "Unknown Start Byte"},
        {RT::ErrorId::BAD_CRC, "CRC Error"},
        {RT::ErrorId::NO_MSC, "No MSC"},
        {RT::ErrorId::MSC_NOT_PRESENT, "MSC Not Present"},
        {RT::ErrorId::MSC_TIMEOUT, "MSC Timeout"},
        {RT::ErrorId::INVALID_MSC, "Invalid MSC"}
    };
};

#endif // FRAME_INTERPRETER_RT_H