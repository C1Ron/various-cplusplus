#include "FrameBuilder.h"
#include <sstream>

std::vector<uint8_t> FrameBuilder::FrameData::complete() 
{
    frame.push_back(calculateCRC());
    return frame;
}

uint8_t FrameBuilder::FrameData::calculateCRC() const 
{
    uint16_t sum = 0;
    for (const auto& byte : frame) {
        sum += byte;
    }
    return static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
}

void FrameBuilder::validateValue(int32_t value, ST_MPC::RegisterType type) 
{
    switch (type) {
        case ST_MPC::RegisterType::UInt8:
            if (value < 0 || value > 255) {
                throw FrameError("Value out of range for UInt8 (0-255)");
            }
            break;
            
        case ST_MPC::RegisterType::Int16:
            if (value < -32768 || value > 32767) {
                throw FrameError("Value out of range for Int16 (-32768 to 32767)");
            }
            break;
            
        case ST_MPC::RegisterType::UInt16:
            if (value < 0 || value > 65535) {
                throw FrameError("Value out of range for UInt16 (0-65535)");
            }
            break;
            
        case ST_MPC::RegisterType::Int32:
            // No range validation needed for Int32
            break;
            
        case ST_MPC::RegisterType::UInt32:
            if (value < 0) {
                throw FrameError("Value must be non-negative for UInt32");
            }
            break;
            
        default:
            throw FrameError("Unknown register type");
    }
}

std::vector<uint8_t> FrameBuilder::valueToBytes(int32_t value, ST_MPC::RegisterType type) 
{
    validateValue(value, type);
    std::vector<uint8_t> bytes;
    
    switch (type) {
        case ST_MPC::RegisterType::UInt8:
            bytes = {static_cast<uint8_t>(value)};
            break;
            
        case ST_MPC::RegisterType::Int16:
        case ST_MPC::RegisterType::UInt16:
            bytes = {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF)
            };
            break;
            
        case ST_MPC::RegisterType::Int32:
        case ST_MPC::RegisterType::UInt32:
            bytes = {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF),
                static_cast<uint8_t>((value >> 16) & 0xFF),
                static_cast<uint8_t>((value >> 24) & 0xFF)
            };
            break;
            
        default:
            throw FrameError("Unknown register type");
    }
    
    return bytes;
}

std::vector<uint8_t> FrameBuilder::buildSetFrame(uint8_t motorId, ST_MPC::RegisterId regId, 
                                                        int32_t value, ST_MPC::RegisterType regType) 
{
    auto valueBytes = valueToBytes(value, regType);
    
    FrameData frame;
    frame.setStartByte((motorId << 5) | static_cast<uint8_t>(ST_MPC::CommandId::SetRegister));
    frame.setPayloadLength(static_cast<uint8_t>(1 + valueBytes.size()));
    frame.addPayloadByte(static_cast<uint8_t>(regId));
    frame.addPayloadBytes(valueBytes);
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilder::buildGetFrame(uint8_t motorId, ST_MPC::RegisterId regId) 
{
    FrameData frame;
    frame.setStartByte((motorId << 5) | static_cast<uint8_t>(ST_MPC::CommandId::GetRegister));
    frame.setPayloadLength(1);
    frame.addPayloadByte(static_cast<uint8_t>(regId));
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilder::buildExecuteFrame(uint8_t motorId, ST_MPC::ExecuteId execId) 
{
    FrameData frame;
    frame.setStartByte((motorId << 5) | static_cast<uint8_t>(ST_MPC::CommandId::Execute));
    frame.setPayloadLength(1);
    frame.addPayloadByte(static_cast<uint8_t>(execId));
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilder::buildRampFrame(uint8_t motorId, int32_t finalSpeed, uint16_t duration) 
{
    FrameData frame;
    frame.setStartByte((motorId << 5) | static_cast<uint8_t>(ST_MPC::CommandId::ExecuteRamp));
    frame.setPayloadLength(6);  // 4 bytes for speed + 2 bytes for duration
    
    // Add final speed bytes (little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(finalSpeed & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 8) & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 16) & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 24) & 0xFF));
    
    // Add duration bytes (little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(duration & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((duration >> 8) & 0xFF));
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilder::buildCurrentFrame(uint8_t motorId, int16_t IqRef, int16_t IdRef)
{
    FrameData frame;
    frame.setStartByte((motorId << 5) | static_cast<uint8_t>(ST_MPC::CommandId::SetCurrentRef));
    frame.setPayloadLength(4);  // 2 bytes for IqRef + 2 bytes for IdRef
    
    // Add IqRef bytes (little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(IqRef & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((IqRef >> 8) & 0xFF));
    
    // Add IdRef bytes (little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(IdRef & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((IdRef >> 8) & 0xFF));
    
    return frame.complete();
}