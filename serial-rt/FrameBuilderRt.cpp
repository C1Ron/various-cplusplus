#include "FrameBuilderRt.h"
#include <sstream>

std::vector<uint8_t> FrameBuilderRt::FrameData::complete() 
{
    frame.push_back(calculateCRC());
    return frame;
}

uint8_t FrameBuilderRt::FrameData::calculateCRC() const 
{
    uint16_t total = 0;
    for(size_t i = 0; i < frame.size(); i++) {
        total += frame[i];
    }
    return static_cast<uint8_t>((total >> 8) + (total & 0x00FF));
}

void FrameBuilderRt::FrameData::addPayloadBytes(const std::vector<uint8_t>& bytes) 
{
    frame.insert(frame.end(), bytes.begin(), bytes.end());
}

void FrameBuilderRt::validateValue(int32_t value, RT::RegisterType type) 
{
    switch (type) {
        case RT::RegisterType::UInt8:
            if (value < 0 || value > 255) {
                throw FrameError("Value out of range for UInt8 (0-255)");
            }
            break;
            
        case RT::RegisterType::Int16:
            if (value < -32768 || value > 32767) {
                throw FrameError("Value out of range for Int16 (-32768 to 32767)");
            }
            break;
            
        case RT::RegisterType::UInt16:
            if (value < 0 || value > 65535) {
                throw FrameError("Value out of range for UInt16 (0-65535)");
            }
            break;
            
        case RT::RegisterType::Int32:
            // No range validation needed for Int32
            break;
            
        case RT::RegisterType::UInt32:
            if (value < 0) {
                throw FrameError("Value must be non-negative for UInt32");
            }
            break;

        case RT::RegisterType::Float:
            // No range validation needed for Float
            break;
            
        case RT::RegisterType::CharPtr:
            throw FrameError("Cannot validate CharPtr as integer value");
            break;
    }
}

std::vector<uint8_t> FrameBuilderRt::valueToBytes(int32_t value, RT::RegisterType type) 
{
    validateValue(value, type);
    std::vector<uint8_t> bytes;
    
    switch (type) {
        case RT::RegisterType::UInt8:
            bytes = {static_cast<uint8_t>(value)};
            break;
            
        case RT::RegisterType::Int16:
        case RT::RegisterType::UInt16:
            bytes = {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF)
            };
            break;
            
        case RT::RegisterType::Int32:
        case RT::RegisterType::UInt32:
            bytes = {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF),
                static_cast<uint8_t>((value >> 16) & 0xFF),
                static_cast<uint8_t>((value >> 24) & 0xFF)
            };
            break;

        case RT::RegisterType::Float: {
            // Interpret the int32_t bits as float
            float float_value = *reinterpret_cast<float*>(&value);
            uint32_t bits = *reinterpret_cast<uint32_t*>(&float_value);
            bytes = {
                static_cast<uint8_t>(bits & 0xFF),
                static_cast<uint8_t>((bits >> 8) & 0xFF),
                static_cast<uint8_t>((bits >> 16) & 0xFF),
                static_cast<uint8_t>((bits >> 24) & 0xFF)
            };
            break;
        }
            
        default:
            throw FrameError("Unsupported register type for value conversion");
    }
    
    return bytes;
}
std::vector<uint8_t> FrameBuilderRt::buildReadFrame(uint8_t mscId, RT::RegisterId regId) 
{
    FrameData frame;
    
    // Field 0: [startByte, totalSize, payloadSize, mscId]
    frame.setStartByte(0xAA);          // startByte always 0xAA
    frame.setTotalSize(0x12);          // total size: 16 (header) + 2 (payload) = 18 bytes (0x12)
    frame.setPayloadSize(0x02);        // payload size: 2 bytes
    frame.addPayloadByte(0x00);        // mscId field (first part)
    
    // Field 1: [msgRequestId, msgResponseId, conversationId, senderId]
    frame.addPayloadByte(0x01);        // msgRequestId
    frame.addPayloadByte(0x00);        // msgResponseId
    frame.addPayloadByte(0x63);        // conversationId
    frame.addPayloadByte(0x01);        // senderId
    
    // Field 2: [numBlocks, seqId, commandType, errorCode]
    frame.addPayloadByte(0x01);        // numBlocks
    frame.addPayloadByte(0x01);        // seqId
    frame.addPayloadByte(static_cast<uint8_t>(RT::CommandId::RT_READ));
    frame.addPayloadByte(static_cast<uint8_t>(RT::ErrorId::NO_ERROR));
    
    // Field 3: [futureUse0, futureUse1, futureUse2, endByte]
    frame.addPayloadByte(0x00);        // futureUse0
    frame.addPayloadByte(0x00);        // futureUse1
    frame.addPayloadByte(0x00);        // futureUse2
    frame.addPayloadByte(0xAA);        // endByte always 0xAA
    
    // Payload
    frame.addPayloadByte(static_cast<uint8_t>(regId));  // Register ID

    auto result = frame.complete();
    std::cout << "FrameBuilder::buildReadFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
    return result;

    // return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildWriteFrame(uint8_t mscId, RT::RegisterId regId, 
                                                    int32_t value, RT::RegisterType regType) 
{
    auto valueBytes = valueToBytes(value, regType);
    
    FrameData frame;
    
    // Header
    frame.setStartByte(0xAA);          // Start byte
    frame.setTotalSize(16 + 1 + valueBytes.size() + 1); // Total size including payload and CRC
    frame.setPayloadSize(1 + valueBytes.size());        // Register ID + value bytes
    frame.addPayloadByte(mscId);       // MSC ID
    frame.addPayloadByte(0);           // Message request ID
    frame.addPayloadByte(0);           // Message response ID
    frame.addPayloadByte(0);           // Conversation ID
    frame.addPayloadByte(0);           // Sender ID
    frame.addPayloadByte(1);           // Number of blocks
    frame.addPayloadByte(0);           // Sequence ID
    frame.addPayloadByte(static_cast<uint8_t>(RT::CommandId::RT_WRITE)); // Command type
    frame.addPayloadByte(static_cast<uint8_t>(RT::ErrorId::NO_ERROR));   // Error code
    frame.addPayloadByte(0);           // Future use 0
    frame.addPayloadByte(0);           // Future use 1
    frame.addPayloadByte(0);           // Future use 2
    frame.addPayloadByte(0xAA);        // End byte
    
    // Payload
    frame.addPayloadByte(static_cast<uint8_t>(regId));
    frame.addPayloadBytes(valueBytes);
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildExecuteFrame(uint8_t mscId, RT::ExecuteId execId) 
{
    FrameData frame;
    
    // Header
    frame.setStartByte(0xAA);          // Start byte
    frame.setTotalSize(16 + 1 + 1);    // Total size including payload and CRC
    frame.setPayloadSize(1);           // Execute command ID
    frame.addPayloadByte(mscId);       // MSC ID
    frame.addPayloadByte(0);           // Message request ID
    frame.addPayloadByte(0);           // Message response ID
    frame.addPayloadByte(0);           // Conversation ID
    frame.addPayloadByte(0);           // Sender ID
    frame.addPayloadByte(1);           // Number of blocks
    frame.addPayloadByte(0);           // Sequence ID
    frame.addPayloadByte(static_cast<uint8_t>(RT::CommandId::RT_EXECUTE)); // Command type
    frame.addPayloadByte(static_cast<uint8_t>(RT::ErrorId::NO_ERROR));     // Error code
    frame.addPayloadByte(0);           // Future use 0
    frame.addPayloadByte(0);           // Future use 1
    frame.addPayloadByte(0);           // Future use 2
    frame.addPayloadByte(0xAA);        // End byte
    
    // Payload
    frame.addPayloadByte(static_cast<uint8_t>(execId));
    
    return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildRampFrame(uint8_t mscId, int32_t finalSpeed, uint16_t duration) 
{
    FrameData frame;
    
    // Header
    frame.setStartByte(0xAA);          // Start byte
    frame.setTotalSize(16 + 6 + 1);    // Total size including payload and CRC
    frame.setPayloadSize(6);           // 4 bytes speed + 2 bytes duration
    frame.addPayloadByte(mscId);       // MSC ID
    frame.addPayloadByte(0);           // Message request ID
    frame.addPayloadByte(0);           // Message response ID
    frame.addPayloadByte(0);           // Conversation ID
    frame.addPayloadByte(0);           // Sender ID
    frame.addPayloadByte(1);           // Number of blocks
    frame.addPayloadByte(0);           // Sequence ID
    frame.addPayloadByte(static_cast<uint8_t>(RT::CommandId::RT_EXECUTE)); // Command type
    frame.addPayloadByte(static_cast<uint8_t>(RT::ErrorId::NO_ERROR));     // Error code
    frame.addPayloadByte(0);           // Future use 0
    frame.addPayloadByte(0);           // Future use 1
    frame.addPayloadByte(0);           // Future use 2
    frame.addPayloadByte(0xAA);        // End byte
    
    // Payload - Final speed (4 bytes, little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(finalSpeed & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 8) & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 16) & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((finalSpeed >> 24) & 0xFF));
    
    // Payload - Duration (2 bytes, little-endian)
    frame.addPayloadByte(static_cast<uint8_t>(duration & 0xFF));
    frame.addPayloadByte(static_cast<uint8_t>((duration >> 8) & 0xFF));
    
    return frame.complete();
}