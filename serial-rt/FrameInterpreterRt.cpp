#include "FrameInterpreterRt.h"
#include <sstream>
#include <iomanip>
#include <iostream>

void FrameInterpreterRt::printResponse(const std::vector<uint8_t>& response) 
{
    for (const auto& byte : response) {
        std::cout << byteToHex(byte) << " ";
    }
    std::cout << std::endl;
}

std::string FrameInterpreterRt::interpretResponse(const std::vector<uint8_t>& response) 
{
    if (response.size() < 16) {
        return "Error: Invalid response size";
    }

    if (response[10] == static_cast<uint8_t>(RT::CommandId::RT_WRITE_REPLY)) {
        // RT_WRITE_REPLY has a bug
        return "Success: Write command executed";
    }

    ResponseInfo info = parseResponse(response);
    
    // Handle bad CRC first
    if (!info.validCRC) {
        return "Error: Invalid CRC";
    }

    // Handle error codes
    if (info.header.errorCode != static_cast<uint8_t>(RT::ErrorId::NO_ERROR)) {
        auto it = errorCodes.find(static_cast<RT::ErrorId>(info.header.errorCode));
        return "Error: " + (it != errorCodes.end() ? it->second : "Unknown error");
    }

    // Route to appropriate handler based on command type
    switch (static_cast<RT::CommandId>(info.header.commandType)) {
        case RT::CommandId::RT_READ_REPLY:
            return handleRtRead(info);
        case RT::CommandId::RT_WRITE_REPLY:
            return handleRtWrite(info);
        case RT::CommandId::RT_EXECUTE_REPLY:
            return handleRtExecute(info);
        case RT::CommandId::FOC_COMMAND_REPLY:
            return handleFocCommand(info);
        default:
            return "Error: Unknown command type";
    }
}

std::string FrameInterpreterRt::interpretResponse(const std::vector<uint8_t>& response, RT::RegisterType type) 
{
    if (response.size() < 16) {
        return "Error: Invalid response size";
    }

    if (response[10] == static_cast<uint8_t>(RT::CommandId::RT_WRITE_REPLY)) {
        return "Success: Write command executed";
    }

    ResponseInfo info = parseResponse(response);
    
    if (!info.validCRC) {
        return "Error: Invalid CRC";
    }

    if (info.header.commandType == static_cast<uint8_t>(RT::CommandId::RT_READ_REPLY)) {
        return handleRtRead(info, type);
    }

    return interpretResponse(response);
}

std::string FrameInterpreterRt::interpretResponse(const std::vector<uint8_t>& response, ST_MPC::RegisterType type) 
{
    if (response.size() < 16) {
        return "Error: Invalid response size";
    }

    if (response[10] == static_cast<uint8_t>(RT::CommandId::RT_WRITE_REPLY)) {
        return "Success: Write command executed";
    }

    ResponseInfo info = parseResponse(response);
    
    if (!info.validCRC) {
        return "Error: Invalid CRC";
    }

    if (info.header.commandType == static_cast<uint8_t>(RT::CommandId::FOC_COMMAND_REPLY)) {
        return handleFocCommand(info, type);
    }

    return interpretResponse(response);
}

FrameInterpreterRt::ResponseInfo FrameInterpreterRt::parseResponse(const std::vector<uint8_t>& response) 
{
    ResponseInfo info;
    info.header = parseHeader(response);

    // Handle special case for RT_WRITE_REPLY
    if (info.header.commandType == static_cast<uint8_t>(RT::CommandId::RT_WRITE_REPLY)) {
        info.validCRC = validateWriteReplyCRC(response);
        return info;
    }

    // Extract payload for normal cases
    if (response.size() > 16) {
        info.payload.assign(response.begin() + 16, response.end() - 1);
        info.crc = response.back();
    }

    info.validCRC = validateCRC(response);
    return info;
}

FrameInterpreterRt::Header FrameInterpreterRt::parseHeader(const std::vector<uint8_t>& response) 
{
    Header header;
    
    // Field 0
    header.startByte = response[0];
    header.totalSize = response[1];
    header.payloadSize = response[2];
    header.mscId = response[3];
    
    // Field 1
    header.msgRequestId = response[4];
    header.msgResponseId = response[5];
    header.conversationId = response[6];
    header.senderId = response[7];
    
    // Field 2
    header.numBlocks = response[8];
    header.seqId = response[9];
    header.commandType = response[10];
    header.errorCode = response[11];
    
    // Field 3
    header.futureUse0 = response[12];
    header.futureUse1 = response[13];
    header.futureUse2 = response[14];
    header.endByte = response[15];
    
    return header;
}

bool FrameInterpreterRt::validateCRC(const std::vector<uint8_t>& frame) const 
{
    if (frame.size() < 2) return false;
    
    uint16_t sum = 0;
    for (size_t i = 0; i < frame.size() - 1; ++i) {
        sum += frame[i];
    }
    
    uint8_t calculated_crc = static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
    return calculated_crc == frame.back();
}

bool FrameInterpreterRt::validateWriteReplyCRC(const std::vector<uint8_t>& frame) const 
{
    if (frame.size() != 16) return false;

    // Create modified frame with endByte as 0xAA and original endByte as CRC
    std::vector<uint8_t> modifiedFrame = frame;
    uint8_t originalCrc = frame[15];
    modifiedFrame[15] = 0xAA;
    modifiedFrame.push_back(originalCrc);

    return validateCRC(modifiedFrame);
}

bool FrameInterpreterRt::validateFocCRC(const std::vector<uint8_t>& focFrame) const 
{
    if (focFrame.size() < 2) return false;
    
    uint16_t sum = 0;
    for (size_t i = 0; i < focFrame.size() - 1; ++i) {
        sum += focFrame[i];
    }
    
    uint8_t calculated_crc = static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
    return calculated_crc == focFrame.back();
}

std::string FrameInterpreterRt::handleRtRead(const ResponseInfo& info, RT::RegisterType type) 
{
    if (info.payload.empty()) {
        return "Error: Empty read response";
    }
    /*
    for (const auto& byte : info.payload) {
        std::cout << byteToHex(byte) << " ";
    }
    std::cout << std::endl;
    */
    return "Success: " + formatValue(info.payload, type);
}

std::string FrameInterpreterRt::handleRtWrite(const ResponseInfo& info) 
{
    // RT_WRITE_REPLY is just an acknowledgment
    return "Success: Write command executed";
}

std::string FrameInterpreterRt::handleRtExecute(const ResponseInfo& info) 
{
    return "Success: Execute command completed";
}

std::string FrameInterpreterRt::handleFocCommand(const ResponseInfo& info) 
{
    if (info.payload.size() < 2) {  // Minimum: mscId + errorCode
        return "Error: Invalid FOC response payload size";
    }

    uint8_t mscId = info.payload[0];
    uint8_t errorCode = info.payload[1];

    // Extract FOC frame: [ack, focPayloadLength, focPayload, focCrc]
    if (info.payload.size() < 4) {  // Minimum FOC frame size
        return "Error: Invalid FOC frame size";
    }

    uint8_t ack = info.payload[2];
    uint8_t focPayloadLength = info.payload[3];
    
    // Check FOC frame validity
    if (ack != static_cast<uint8_t>(ST_MPC::AckStatus::Success)) {
        return "Error: FOC command failed, status: " + byteToHex(ack);
    }

    // Validate FOC CRC
    std::vector<uint8_t> focFrame(info.payload.begin() + 2, info.payload.end());
    if (!validateFocCRC(focFrame)) {
        return "Error: Invalid FOC CRC";
    }

    // If it's a write response, we're done
    if (focPayloadLength == 0) {
        return "Success: FOC command executed";
    }

    // For read responses, extract the raw value
    std::vector<uint8_t> focPayload(info.payload.begin() + 4, 
                                   info.payload.begin() + 4 + focPayloadLength);
    
    std::stringstream ss;
    ss << "Success: Raw FOC value = ";
    for (const auto& byte : focPayload) {
        ss << byteToHex(byte) << " ";
    }
    
    return ss.str();
}

std::string FrameInterpreterRt::handleFocCommand(const ResponseInfo& info, ST_MPC::RegisterType type) 
{
    if (info.payload.size() < 2) {
        return "Error: Invalid FOC response payload size";
    }

    uint8_t mscId = info.payload[0];
    uint8_t errorCode = info.payload[1];

    if (info.payload.size() < 4) {
        return "Error: Invalid FOC frame size";
    }

    uint8_t ack = info.payload[2];
    uint8_t focPayloadLength = info.payload[3];
    
    if (ack != static_cast<uint8_t>(ST_MPC::AckStatus::Success)) {
        return "Error: FOC command failed, status: " + byteToHex(ack);
    }

    std::vector<uint8_t> focFrame(info.payload.begin() + 2, info.payload.end());
    if (!validateFocCRC(focFrame)) {
        return "Error: Invalid FOC CRC";
    }

    if (focPayloadLength == 0) {
        return "Success: FOC command executed";
    }

    // Extract value and format according to type
    std::vector<uint8_t> focPayload(info.payload.begin() + 4, 
                                   info.payload.begin() + 4 + focPayloadLength);
    
    return "Success: " + formatFocValue(focPayload, type);
}

std::string FrameInterpreterRt::formatValue(const std::vector<uint8_t>& payload, RT::RegisterType type) const 
{
    if (payload.empty()) return "No data";
    
    std::stringstream ss;

    // Skip register ID byte for value extraction
    const uint8_t* valueBytes = payload.data() + 1;
    size_t valueBytesSize = payload.size() - 1;
    
    switch (type) {
        case RT::RegisterType::UInt8: {
            if (valueBytesSize < 1) return "Error: Invalid payload size for UInt8";
            uint8_t value = valueBytes[0];
            ss << "Value: " << static_cast<int>(value);
            break;
        }
        case RT::RegisterType::Int16: {
            if (valueBytesSize < 2) return "Error: Invalid payload size for Int16";
            int16_t value = static_cast<int16_t>(valueBytes[0] | (valueBytes[1] << 8));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case RT::RegisterType::UInt16: {
            if (valueBytesSize < 2) return "Error: Invalid payload size for UInt16";
            uint16_t value = static_cast<uint16_t>(valueBytes[0] | (valueBytes[1] << 8));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case RT::RegisterType::Int32: {
            if (valueBytesSize < 4) return "Error: Invalid payload size for Int32";
            int32_t value = static_cast<int32_t>(valueBytes[0] | 
                                              (valueBytes[1] << 8) |
                                              (valueBytes[2] << 16) |
                                              (valueBytes[3] << 24));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case RT::RegisterType::UInt32: {
            if (valueBytesSize < 4) return "Error: Invalid payload size for UInt32";
            uint32_t value = static_cast<uint32_t>(valueBytes[0] |
                                               (valueBytes[1] << 8) |
                                               (valueBytes[2] << 16) |
                                               (valueBytes[3] << 24));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case RT::RegisterType::Float: {
            if (valueBytesSize < 4) return "Error: Invalid payload size for Float";
            uint32_t bits = (valueBytes[0] |
                          (valueBytes[1] << 8) |
                          (valueBytes[2] << 16) |
                          (valueBytes[3] << 24));  // Little-endian
            float value = *reinterpret_cast<const float*>(&bits);
            ss << "Value: " << std::fixed << std::setprecision(6) << value;
            break;
        }
        case RT::RegisterType::CharPtr: {
            ss << "String: " << std::string(valueBytes, valueBytes + valueBytesSize);
            break;
        }
        default: {
            ss << "Raw data: ";
            for (size_t i = 0; i < valueBytesSize; ++i) {
                ss << byteToHex(valueBytes[i]) << " ";
            }
            break;
        }
    }
    
    return ss.str();
}

std::string FrameInterpreterRt::formatFocValue(const std::vector<uint8_t>& payload, ST_MPC::RegisterType type) const 
{
    if (payload.empty()) return "No data";
    
    std::stringstream ss;
    
    switch (type) {
        case ST_MPC::RegisterType::UInt8: {
            if (payload.size() < 1) return "Error: Invalid payload size for UInt8";
            uint8_t value = payload[0];
            ss << "Value: " << static_cast<int>(value);
            break;
        }
        case ST_MPC::RegisterType::Int16: {
            if (payload.size() < 2) return "Error: Invalid payload size for Int16";
            int16_t value = static_cast<int16_t>(payload[0] | (payload[1] << 8));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case ST_MPC::RegisterType::UInt16: {
            if (payload.size() < 2) return "Error: Invalid payload size for UInt16";
            uint16_t value = static_cast<uint16_t>(payload[0] | (payload[1] << 8));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case ST_MPC::RegisterType::Int32: {
            if (payload.size() < 4) return "Error: Invalid payload size for Int32";
            int32_t value = static_cast<int32_t>(payload[0] | 
                                              (payload[1] << 8) |
                                              (payload[2] << 16) |
                                              (payload[3] << 24));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case ST_MPC::RegisterType::UInt32: {
            if (payload.size() < 4) return "Error: Invalid payload size for UInt32";
            uint32_t value = static_cast<uint32_t>(payload[0] |
                                               (payload[1] << 8) |
                                               (payload[2] << 16) |
                                               (payload[3] << 24));  // Little-endian
            ss << "Value: " << value;
            break;
        }
        case ST_MPC::RegisterType::CharPtr: {
            ss << "String: " << std::string(payload.begin(), payload.end());
            break;
        }
        default: {
            ss << "Raw data: ";
            for (const auto& byte : payload) {
                ss << byteToHex(byte) << " ";
            }
            break;
        }
    }
    
    return ss.str();
}

std::string FrameInterpreterRt::byteToHex(uint8_t byte) 
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return ss.str();
}
