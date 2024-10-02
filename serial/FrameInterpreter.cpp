// FrameInterpreter.cpp
#include "FrameInterpreter.h"
#include <sstream>

void FrameInterpreter::printResponse(const std::vector<uint8_t>& response) 
{
    for (const auto& byte : response) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}
std::string FrameInterpreter::interpretResponse(const std::vector<uint8_t>& response) 
{
    if (response.size() < 3) {
        return "Invalid response size.";
    }

    if (response[0] == 0xF0) {
        size_t payloadLength = response[1];
        if (payloadLength == 0) {
            return "Success. No payload.";
        } else if (payloadLength == 1) {
            return "Success. Payload: " + std::to_string(response[2]);
        } else if (payloadLength == 2) {
            return "Success. Payload: " + std::to_string(response[2] + (response[3] << 8));
        } else if (payloadLength == 4) {
            return "Success. Payload: " + std::to_string(response[2] + (response[3] << 8) + (response[4] << 16) + (response[5] << 24));
        } else {
            return "Invalid payload length.";
        }
    } else if (response[0] == 0xFF) {
        std::stringstream ss;
        ss << "Error. Code: 0x" << std::hex << static_cast<int>(response[2]);
        return ss.str();
    }

    return "Unknown response format.";
}
