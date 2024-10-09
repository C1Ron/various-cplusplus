#include "FrameInterpreter.h"
#include <sstream>
#include <iomanip>
#include <iostream>


std::string FrameInterpreter::interpretResponse(const std::vector<uint8_t>& response) 
{
    if (response.size() < 3) {
        return "Invalid response size.";
    }

    uint8_t startFrame = response[0];
    uint8_t payloadLength = response[1];

    if (response.size() != static_cast<size_t>(payloadLength) + 3) { // start + length + payload + CRC
        return "Invalid response length.";
    }

    if (startFrame == 0xF0) {
        return interpretSuccessResponse(response);
    } else if (startFrame == 0xFF) {
        return interpretErrorResponse(response);
    }

    return "Unknown response format.";
}

void FrameInterpreter::printResponse(const std::vector<uint8_t>& response) const
{
    for (const auto& byte : response) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}

std::string FrameInterpreter::interpretSuccessResponse(const std::vector<uint8_t>& response) const
{
    uint8_t payloadLength = response[1];
    std::stringstream ss;
    ss << "Success. Payload: ";

    if (payloadLength == 0) {
        ss << "No payload.";
    } else {
        uint32_t value = 0;
        for (size_t i = 0; i < payloadLength && i < 4; ++i) {
            value |= static_cast<uint32_t>(response[2 + i]) << (8 * i);
        }
        ss << value;
    }

    return ss.str();
}

std::string FrameInterpreter::interpretErrorResponse(const std::vector<uint8_t>& response) const
{
    std::stringstream ss;
    ss << "Error. Code: 0x" << std::hex << static_cast<int>(response[2]);
    return ss.str();
}

std::string FrameInterpreter::byteToHexString(uint8_t byte) const
{
    std::stringstream ss;
    ss << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(byte);
    return ss.str();
}