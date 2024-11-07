#include "FrameInterpreter.h"
#include <sstream>
#include <iomanip>
#include <iostream>

void FrameInterpreter::printResponse(const std::vector<uint8_t>& response) {
    for (const auto& byte : response) {
        std::cout << byteToHex(byte) << " ";
    }
    std::cout << std::endl;
}

std::string FrameInterpreter::interpretResponse(const std::vector<uint8_t>& response) {
    if (response.size() < 3) {  // Minimum frame size: start + length + CRC
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

FrameInterpreter::ResponseInfo FrameInterpreter::parseResponse(const std::vector<uint8_t>& response) {
    ResponseInfo info;
    info.isSuccess = (response[0] == 0xF0);
    info.payloadLength = response[1];
    
    // Validate frame size
    if (response.size() != static_cast<size_t>(info.payloadLength) + 3) {  // start + length + payload + CRC
        info.validCRC = false;
        return info;
    }

    // Extract payload
    info.payload.assign(response.begin() + 2, response.end() - 1);
    
    // Validate CRC
    info.validCRC = validateCRC(response);
    
    return info;
}

bool FrameInterpreter::validateCRC(const std::vector<uint8_t>& frame) {
    if (frame.empty()) return false;
    
    uint16_t sum = 0;
    for (size_t i = 0; i < frame.size() - 1; ++i) {
        sum += frame[i];
    }
    
    uint8_t calculated_crc = static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
    return calculated_crc == frame.back();
}

std::string FrameInterpreter::interpretSuccessResponse(const ResponseInfo& info) {
    if (info.payloadLength == 0) {
        return "Success: Command executed";
    }
    
    std::string valueStr = formatValue(info.payload);
    return "Success: " + valueStr;
}

std::string FrameInterpreter::interpretErrorResponse(const ResponseInfo& info) {
    if (info.payload.empty()) {
        return "Error: Unknown error occurred";
    }
    
    uint8_t errorCode = info.payload[0];
    auto it = errorCodes.find(errorCode);
    if (it != errorCodes.end()) {
        return "Error: " + it->second;
    }
    
    return "Error: Unknown error code 0x" + byteToHex(errorCode);
}

std::string FrameInterpreter::formatValue(const std::vector<uint8_t>& payload) {
    if (payload.empty()) return "No data";
    
    std::stringstream ss;
    ss << "Value: ";
    
    switch (payload.size()) {
        case 1: {
            uint8_t value = payload[0];
            ss << static_cast<int>(value);
            ss << " (0x" << byteToHex(value) << ")";
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

std::string FrameInterpreter::byteToHex(uint8_t byte) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return ss.str();
}