#ifndef ST_MPC_DEFINITIONS_H
#define ST_MPC_DEFINITIONS_H

#include <cstdint>

namespace ST_MPC
{
    enum class FrameIndex : uint8_t
    {
        StartFrame = 0,
        PayloadLength = 1,
        FrameId = 2,
        Payload = 3,
        Crc = 4
    };

    enum class CommandId : uint8_t
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

    enum class RegisterId : uint8_t
    {
        TargetMotor = 0x00,    // U8
        Flags = 0x01,          // U32
        Status = 0x02,         // U8
        ControlMode = 0x03,    // U8
        SpeedRef = 0x04,       // S32
        SpeedKp = 0x05,        // U16 (bug in SDK: is S16)
        SpeedKi = 0x06,        // U16 (bug in SDK: is S16)
        SpeedKd = 0x07,        // U16 (bug in SDK: is S16)
        TorqueRef = 0x08,      // S16
        TorqueKp = 0x09,       // U16 (bug in SDK: is S16)
        TorqueKi = 0x0A,       // U16 (bug in SDK: is S16)
        TorqueKd = 0x0B,       // U16 (bug in SDK: is S16)
        FluxRef = 0x0C,        // S16
        FluxKp = 0x0D,         // U16 (bug in SDK: is S16)
        FluxKi = 0x0E,         // U16 (bug in SDK: is S16)
        FluxKd = 0x0F,         // U16 (bug in SDK: is S16)
        BusVoltage = 0x19,     // U16
        MotorPower = 0x1B,     // U16
        SpeedMeas = 0x1E,      // S32
        TorqueMeas = 0x1F,     // S16
        FluxMeas = 0x20,       // S16
        Ia = 0x23,             // S16
        Ib = 0x24,             // S16
        Ialpha = 0x25,         // S16
        Ibeta = 0x26,          // S16
        Iq = 0x27,             // S16
        Id = 0x28,             // S16
        IqRef = 0x29,          // S16
        IdRef = 0x2A,          // S16
        Vq = 0x2B,             // S16
        Vd = 0x2C,             // S16
        Valpha = 0x2D,         // S16
        Vbeta = 0x2E,          // S16
        ElAngleMeas = 0x2F,    // S16
        IqRefSpeedMode = 0x41, // S16
        RampFinalSpeed = 0x5B, // S32
        RampDuration = 0x5C,   // U16
        SpeedKpDiv = 0x6E,     // U16
        SpeedKiDiv = 0x6F,     // U16
        TransDetReg1000 = 0xC8, // U8
        TransDetReg1200 = 0xC9, // U8
        TransDetReg1300 = 0xCA, // U8
        TransDetRegId = 0xCB,   // U8
        DeadTimeRegId = 0xD4,   // U8
        DeadTimeRegA = 0XD5,    // U8
        DeadTimeRegB = 0xD6,    // U8
        GdrPwrDis = 0xD7,       // U8
        GdrPwmEn = 0xD8,        // U8
        GdrFltPhA = 0xDC,       // U8
        GdrFltPhB = 0xDD,       // U8
        GdrFltPhC = 0xDE,       // U8
        GdrTempPhA = 0xDF,      // U32
        GdrTempPhB = 0xE0,      // U32
        GdrTempPhC = 0xE1,      // U32
        MuxRegId =  0xE2,       // U8
        TorqueKpDivPow2 = 0xE3, // U16
        TorqueKiDivPow2 = 0xE4, // U16
        FluxKpDivPow2 = 0xE5,   // U16
        FluxKiDivPow2 = 0xE6,   // U16
        SpeedKpDivPow2 = 0xE7,  // U16
        SpeedKiDivPow2 = 0xE8,  // U16
        TorqueKpDiv = 0xE9,     // U16
        TorqueKiDiv = 0xEA,     // U16
        FluxKpDiv = 0xEB,       // U16
        FluxKiDiv = 0xEC,       // U16
        AlignFinalFlux = 0xED,  // U16
        AlignRampUpDuration = 0xEE,   // U16
        AlignRampDownDuration = 0xEF, // U16
        IsAligned = 0xF0,       // U16 ?
        GitVersion = 0xF1       // char *


    };

    enum class RegisterType : uint8_t
    {
        UInt8,
        Int16,
        UInt16,
        Int32,
        UInt32,
        CharPtr
    };

    enum class ExecuteId : uint8_t
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

    enum class AckStatus : uint8_t
    {
        Success = 0xF0,
        Failure = 0xFF
    };

    enum class AckErrorId : uint8_t
    {
        FrameId = 0x01,
        SetReadOnly = 0x02,
        GetWriteOnly = 0x03,
        NoTargetMotor = 0x04,
        OutOfRange = 0x05,
        BadCrc = 0x0A
    };

    enum class Status : uint8_t
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

    enum class ControlMode : uint8_t
    {
        Torque = 0,
        Speed = 1
    };

}; // namespace ST_MPC

#endif // ST_MPC_DEFINITIONS_H