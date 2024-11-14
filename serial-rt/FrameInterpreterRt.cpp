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
    if (response.size() < 17) {  // Minimum frame size: 16 byte header + 1 byte CRC
        return "Error: Invalid response size";
    }

    auto info = parseResponse(response);
    if (!info.validCRC) {
        return "Error: Invalid CRC";
    }

    if (info.isSuccess) {
        return interpretSuccessResponse(info);
    } else {
        return interpretErrorResponse(info);
    }
}

std::string FrameInterpreterRt::interpretResponse(const std::vector<uint8_t>& response, RT::RegisterType type) 
{
    if (response.size() < 17) {  // Minimum frame size: 16 byte header + 1 byte CRC
        return "Error: Invalid response size";
    }

    auto info = parseResponse(response);
    if (!info.validCRC) {
        return "Error: Invalid CRC";
    }

    if (info.isSuccess) {
        return interpretSuccessResponse(info, type);
    } else {
        return interpretErrorResponse(info);
    }
}

FrameInterpreterRt::ResponseInfo FrameInterpreterRt::parseResponse(const std::vector<uint8_t>& response) 
{
    ResponseInfo info;
    
    // Extract header fields
    uint8_t commandType = response[10];  // Command type from header
    uint8_t errorCode = response[11];    // Error code from header
    
    // Set success based on error code
    info.isSuccess = (static_cast<RT::ErrorId>(errorCode) == RT::ErrorId::NO_ERROR);
    
    // Get payload size from header
    info.payloadLength = response[2];
    
    // Extract payload (skip 16-byte header, exclude CRC)
    if (response.size() > 16) {
        info.payload.assign(response.begin() + 16, response.end() - 1);
    }
    
    // Validate CRC
    info.validCRC = validateCRC(response);
    
    return info;
}

bool FrameInterpreterRt::validateCRC(const std::vector<uint8_t>& frame) 
{
    if (frame.empty()) return false;
    
    uint16_t sum = 0;
    for (size_t i = 0; i < frame.size() - 1; ++i) {
        sum += frame[i];
    }
    
    uint8_t calculated_crc = static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
    return calculated_crc == frame.back();
}

std::string FrameInterpreterRt::interpretSuccessResponse(const ResponseInfo& info)
{
    if (info.payloadLength == 0) {
        return "Success: Command executed";
    }
    
    std::string valueStr = formatValue(info.payload);
    return "Success: " + valueStr;
}

std::string FrameInterpreterRt::interpretSuccessResponse(const ResponseInfo& info, RT::RegisterType type)
{
    if (info.payloadLength == 0) {
        return "Success: Command executed";
    }
    
    std::string valueStr = formatValue(info.payload, type);
    return "Success: " + valueStr;
}

std::string FrameInterpreterRt::interpretErrorResponse(const ResponseInfo& info) 
{
    if (info.payload.empty()) {
        return "Error: Unknown error occurred";
    }
    
    RT::ErrorId errorCode = static_cast<RT::ErrorId>(info.payload[0]);
    auto it = errorCodes.find(errorCode);
    if (it != errorCodes.end()) {
        return "Error: " + it->second;
    }
    
    return "Error: Unknown error code " + byteToHex(info.payload[0]);
}

std::string FrameInterpreterRt::formatValue(const std::vector<uint8_t>& payload) 
{
    if (payload.empty()) return "No data";
    
    std::stringstream ss;
    ss << "Value: ";
    
    switch (payload.size()) {
        case 1: {
            uint8_t value = payload[0];
            ss << static_cast<int>(value);
            ss << " (" << byteToHex(value) << ")";
            break;
        }
        case 2: {
            int16_t value = static_cast<int16_t>(payload[0] | (payload[1] << 8));
            ss << value;
            ss << " (0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(value) << ")";
            break;
        }
        case 4: {
            int32_t value = static_cast<int32_t>(payload[0] | (payload[1] << 8) | 
                                                (payload[2] << 16) | (payload[3] << 24));
            ss << value;
            ss << " (0x" << std::hex << std::setw(8) << std::setfill('0') << value << ")";
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

std::string FrameInterpreterRt::formatValue(const std::vector<uint8_t>& payload, RT::RegisterType type) 
{
    if (payload.empty()) return "No data";
    
    std::stringstream ss;
    ss << "Value: ";
    
    switch (type) {
        case RT::RegisterType::UInt8: {
            if (payload.size() < 1) return "Error: Invalid payload size for UInt8";
            uint8_t value = payload[0];
            ss << static_cast<int>(value);
            ss << " (" << byteToHex(value) << ")";
            break;
        }
        case RT::RegisterType::UInt16: {
            if (payload.size() < 2) return "Error: Invalid payload size for UInt16";
            uint16_t value = static_cast<uint16_t>(payload[0] | (payload[1] << 8));
            ss << value;
            ss << " (0x" << std::hex << std::setw(4) << std::setfill('0') << value << ")";
            break;
        }
        case RT::RegisterType::Int16: {
            if (payload.size() < 2) return "Error: Invalid payload size for Int16";
            int16_t value = static_cast<int16_t>(payload[0] | (payload[1] << 8));
            ss << value;
            ss << " (0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(value) << ")";
            break;
        }
        case RT::RegisterType::UInt32: {
            if (payload.size() < 4) return "Error: Invalid payload size for UInt32";
            uint32_t value = static_cast<uint32_t>(payload[0] | (payload[1] << 8) | 
                                                (payload[2] << 16) | (payload[3] << 24));
            ss << value;
            ss << " (0x" << std::hex << std::setw(8) << std::setfill('0') << value << ")";
            break;
        }
        case RT::RegisterType::Int32: {
            if (payload.size() < 4) return "Error: Invalid payload size for Int32";
            int32_t value = static_cast<int32_t>(payload[0] | (payload[1] << 8) | 
                                                (payload[2] << 16) | (payload[3] << 24));
            ss << value;
            ss << " (0x" << std::hex << std::setw(8) << std::setfill('0') << value << ")";
            break;
        }
        case RT::RegisterType::Float: {
            if (payload.size() < 4) return "Error: Invalid payload size for Float";
            // Reconstruct float from bytes
            uint32_t bits = static_cast<uint32_t>(payload[0] | (payload[1] << 8) | 
                                                 (payload[2] << 16) | (payload[3] << 24));
            float value = *reinterpret_cast<float*>(&bits);
            ss << std::fixed << std::setprecision(6) << value;
            break;
        }
        case RT::RegisterType::CharPtr: {
            std::string str(payload.begin(), payload.end());
            ss << str;
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