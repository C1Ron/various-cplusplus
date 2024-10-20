#pragma once
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <variant>

class VirtualMsc 
{
public:
    VirtualMsc();
    std::vector<uint8_t> processFrame(const std::vector<uint8_t>& frame);

private:
    struct Register {
        std::variant<int16_t, int32_t> value;
        bool is32bit;
    };

    std::map<uint8_t, Register> registers;

    uint8_t calculateCRC(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> createSuccessResponse(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> createErrorResponse(uint8_t errorCode);
    std::vector<uint8_t> handleSetCommand(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> handleGetCommand(const std::vector<uint8_t>& payload);
    void debugPrint(const std::vector<uint8_t>& frame, const std::string& label);
};

inline constexpr uint8_t SUCCESS_FRAME_ACK = 0xF0;
inline constexpr uint8_t FAILURE_FRAME_ACK = 0xFF;
inline constexpr uint8_t SET_COMMAND = 0x01;
inline constexpr uint8_t GET_COMMAND = 0x02;
inline constexpr uint8_t EXECUTE_COMMAND = 0x03;
inline constexpr uint8_t RAMP_COMMAND = 0x07;