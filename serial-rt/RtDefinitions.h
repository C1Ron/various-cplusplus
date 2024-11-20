#ifndef RT_DEFINITIONS_H
#define RT_DEFINITIONS_H

#include <cstdint>

namespace RT 
{
    enum class RegisterId : uint8_t 
    {
        RAMP_FINAL_SPEED = 0x01,        // INT32
        RAMP_DURATION = 0x02,           // UINT16
        SPEED_SETPOINT = 0x03,          // FLOAT
        SPEED_KP = 0x04,                // FLOAT
        SPEED_KI = 0x05,                // FLOAT
        SPEED_KD = 0x06,                // FLOAT
        BOARD_INFO = 0x07,              // char *
        CURRENT_SPEED = 0x08,           // FLOAT
        SPEED_LOOP_PERIOD_MS = 0x09,    // UINT32
        GIT_VERSION = 0x10              // char *
    };

    enum class RegisterType : uint8_t 
    {
        UInt8,
        Int16,
        UInt16,
        Int32,
        UInt32,
        Float,
        CharPtr
    };

    enum class CommandId : uint8_t 
    {
        RT_EXECUTE = 0,
        RT_EXECUTE_REPLY = 1,
        RT_WRITE = 2,
        RT_WRITE_REPLY = 3,
        RT_READ = 4,
        RT_READ_REPLY = 5,
        FOC_COMMAND = 6,
        FOC_COMMAND_REPLY = 7
    };

    enum class ExecuteId : uint8_t 
    {
        START_MOTOR = 0x1,
        STOP_MOTOR = 0x2,
        RAMP_EXECUTE = 0x3,
        START_FEEDBACK = 0x4,
        STOP_FEEDBACK = 0x5
    };

    enum class ErrorId : uint8_t 
    {
        UNKNOWN_START_BYTE = 1,
        BAD_CRC = 2,
        NO_MSC = 3,
        MSC_NOT_PRESENT = 4,
        MSC_TIMEOUT = 5,
        INVALID_MSC = 6,
        NO_ERROR = 7
    };
}

#endif // RT_DEFINITIONS_H