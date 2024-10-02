#ifndef ST_MPC_DEFINITIONS_H
#define ST_MPC_DEFINITIONS_H

namespace ST_MPC
{
    enum class FrameIndex
    {
        StartFrame = 0,
        PayloadLength = 1,
        FrameId = 2,
        Payload = 3,
        Crc = 4
    };

    enum class CommandId
    {
        SetRegister = 0x01,
        GetRegister = 0x02,
        Execute = 0x03,
        GetInfo = 0x06,
        ExecuteRamp = 0x07,
        GetRevup = 0x08,
        SetRevup = 0x09,
        SetCurrentRef = 0x0A
    };

    enum class RegisterId
    {
        TargetMotor = 0x00,    // U8
        Flags = 0x01,          // U32
        Status = 0x02,         // U8
        ControlMode = 0x03,    // U8
        SpeedRef = 0x04,       // S32
        SpeedKp = 0x05,        // U16
        SpeedKi = 0x06,        // U16
        SpeedKd = 0x07,        // U16
        TorqueRef = 0x08,      // S16
        TorqueKp = 0x09,       // U16
        TorqueKi = 0x0A,       // U16
        TorqueKd = 0x0B,       // U16
        FluxRef = 0x0C,        // S16
        FluxKp = 0x0D,         // U16
        FluxKi = 0x0E,         // U16
        FluxKd = 0x0F,         // U16
        MotorPower = 0x1B,     // U16
        SpeedMeas = 0x1E,      // S32
        TorqueMeas = 0x1F,     // S16
        FluxMeas = 0x20,       // S16
        RampFinalSpeed = 0x5B, // S32
        RampDuration = 0x5C    // U16 
    };

    enum class RegisterType
    {
        uint8_t,
        int16_t,
        uint16_t,
        int32_t,
        uint32_t
    };

    enum class ExecuteId
    {
        StartMotor = 0x01,
        StopMotor = 0x02,
        StopRamp = 0x03,
        Reset = 0x04,
        Ping = 0x05,
        StartStop = 0x06,
        FaultAck = 0x07,
        EncoderAlign = 0x08
    };

    enum class AckStatus
    {
        Success = 0xF0,
        Failure = 0xFF
    };

    enum class AckErrorId
    {
        FrameId = 0x01,
        SetReadOnly = 0x02,
        GetWriteOnly = 0x03,
        NoTargetMotor = 0x04,
        OutOfRange = 0x05,
        BadCrc = 0x0A
    };

    enum class Status
    {
        Idle = 0x00,
        IdleAlignment = 0x01,
        Alignment = 0x02,
        IdleStart = 0x03,
        Start = 0x04,
        StartRun = 0x05,
        Run = 0x06,
        AnyStop = 0x07,
        Stop = 0x08,
        StopIdle = 0x09,
        FaultNow = 0x0A,
        FaultOver = 0x0B
    };

};

#endif // ST_MPC_DEFINITIONS_H