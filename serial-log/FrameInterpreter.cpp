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

    // Example logic to determine whether to treat the payload as int16 or int32
    // You will need to replace this logic with your own based on your application
    bool isInt16 = (startFrame == 0xF0 && payloadLength == 2); // Adjust as needed

    if (startFrame == 0xF0) {
        return interpretSuccessResponse(response, isInt16);
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

std::string FrameInterpreter::interpretSuccessResponse(const std::vector<uint8_t>& response, bool isInt16) const
{
    uint8_t payloadLength = response[1];
    std::stringstream ss;
    ss << "Success. Payload: ";

    if (payloadLength == 0) {
        ss << "No payload.";
    } else {
        if (isInt16) {
            // Handle signed 16-bit integer interpretation
            if (payloadLength != 2) {
                ss << "Invalid payload length for int16.";
                return ss.str();
            }

            // Combine bytes into a signed 16-bit integer (little-endian)
            int16_t value = (static_cast<int16_t>(response[2 + 1]) << 8) | static_cast<int16_t>(response[2]);

            ss << "Raw payload: ";
            for (size_t i = 2; i < 4; ++i) {
                ss << std::hex << "0x" << static_cast<int>(response[i]) << " "; // Keep hex for raw payload
            }

            // Output the interpreted value
            ss << "\nInterpreted value: " << std::dec << value;  // Show as decimal

        } else {
            // Handle signed 32-bit integer interpretation
            uint32_t unsignedValue = 0;

            for (size_t i = 0; i < static_cast<size_t>(payloadLength) && i < 4; ++i) {
                unsignedValue |= static_cast<uint32_t>(response[2 + i]) << (8 * i);
            }

            // Convert to signed integer
            int32_t value = static_cast<int32_t>(unsignedValue);
            if (value < 0) {
                value -= 4294967296;  // Adjust for two's complement (2^32)
            }

            // Print raw payload for debugging
            ss << "Raw payload: ";
            for (size_t i = 2; i < 2 + static_cast<size_t>(payloadLength); ++i) {
                ss << std::hex << "0x" << static_cast<int>(response[i]) << " "; // Keep hex for raw payload
            }

            // Print the interpreted value in decimal
            ss << "\nInterpreted value: " << std::dec << value;  // Show as decimal
        }
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
