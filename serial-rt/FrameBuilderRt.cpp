#include "FrameBuilderRt.h"
#include <sstream>

std::vector<uint8_t> FrameBuilderRt::FrameData::complete() 
{
    frame.push_back(calculateCRC());
    return frame;
}

uint8_t FrameBuilderRt::FrameData::calculateCRC() const 
{
    uint16_t sum = 0;
    for(size_t i = 0; i < frame.size(); i++) {
        sum += frame[i];
    }
    return static_cast<uint8_t>((sum >> 8) + (sum & 0x00FF));
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
        case RT::RegisterType::UInt16: {
            // Little-endian: LSB first
            bytes = {
                static_cast<uint8_t>(value & 0xFF),         // LSB
                static_cast<uint8_t>((value >> 8) & 0xFF)   // MSB
            };
            break;
        }
            
        case RT::RegisterType::Int32:
        case RT::RegisterType::UInt32: {
            // Little-endian: LSB first
            bytes = {
                static_cast<uint8_t>(value & 0xFF),           // LSB
                static_cast<uint8_t>((value >> 8) & 0xFF),
                static_cast<uint8_t>((value >> 16) & 0xFF),
                static_cast<uint8_t>((value >> 24) & 0xFF)    // MSB
            };
            break;
        }

        case RT::RegisterType::Float: {
            union FloatConverter {
                float f;
                uint32_t i;
            };
            FloatConverter converter;
            converter.f = *reinterpret_cast<float*>(&value);
            uint32_t bits = converter.i;
            // Little-endian: LSB first
            bytes = {
                static_cast<uint8_t>(bits & 0xFF),           // LSB
                static_cast<uint8_t>((bits >> 8) & 0xFF),
                static_cast<uint8_t>((bits >> 16) & 0xFF),
                static_cast<uint8_t>((bits >> 24) & 0xFF)    // MSB
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

    /*
    auto result = frame.complete();
    std::cout << "FrameBuilder::buildReadFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
    return result; 
    */

    return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildWriteFrame(uint8_t mscId, RT::RegisterId regId, 
                                                    int32_t value, RT::RegisterType regType) 
{
    auto valueBytes = valueToBytes(value, regType);
    
    // Set sizes based on register type
    uint8_t payloadSize;
    uint8_t totalSize;
    
    switch (regType) {
        case RT::RegisterType::Float:
            payloadSize = 0x06;  // regId(1) + float value(4) + CRC(1)
            totalSize = 0x16;    // header(16) + payload(6)
            break;
        case RT::RegisterType::UInt16:
            payloadSize = 0x04;  // regId(1) + uint16 value(2) + CRC(1)
            totalSize = 0x14;    // header(16) + payload(4)
            break;
        // Add other cases as needed
        default:
            throw FrameError("Unsupported register type for write");
    }
    
    FrameData frame;
    
    // Header
    frame.setStartByte(0xAA);
    frame.setTotalSize(totalSize);
    frame.setPayloadSize(payloadSize);
    frame.addPayloadByte(0x00);        // MSC ID first part
    frame.addPayloadByte(0x0a);        // Message request ID
    frame.addPayloadByte(0x00);        // Message response ID
    frame.addPayloadByte(0x63);        // Conversation ID
    frame.addPayloadByte(0x01);        // Sender ID
    frame.addPayloadByte(0x01);        // Number of blocks
    frame.addPayloadByte(0x01);        // Sequence ID
    frame.addPayloadByte(static_cast<uint8_t>(RT::CommandId::RT_WRITE));
    frame.addPayloadByte(static_cast<uint8_t>(RT::ErrorId::NO_ERROR));
    frame.addPayloadByte(0x00);        // Future use 0
    frame.addPayloadByte(0x00);        // Future use 1
    frame.addPayloadByte(0x00);        // Future use 2
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

    /*
    auto result = frame.complete();
    std::cout << "FrameBuilder::buildExecuteFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;

    return result;
    */

    return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::valueToBytes(int32_t value, ST_MPC::RegisterType type)
{
    switch (type) {
        case ST_MPC::RegisterType::UInt8:
            validateValue(value, RT::RegisterType::UInt8);
            return {static_cast<uint8_t>(value)};
            
        case ST_MPC::RegisterType::Int16:
            validateValue(value, RT::RegisterType::Int16);
            return {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF)
            };
            
        case ST_MPC::RegisterType::UInt16:
            validateValue(value, RT::RegisterType::UInt16);
            return {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF)
            };
            
        case ST_MPC::RegisterType::Int32:
        case ST_MPC::RegisterType::UInt32:
            validateValue(value, RT::RegisterType::Int32);
            return {
                static_cast<uint8_t>(value & 0xFF),
                static_cast<uint8_t>((value >> 8) & 0xFF),
                static_cast<uint8_t>((value >> 16) & 0xFF),
                static_cast<uint8_t>((value >> 24) & 0xFF)
            };
            
        case ST_MPC::RegisterType::CharPtr:
            throw FrameError("Cannot convert value to CharPtr type");
            
        default:
            throw FrameError("Unknown ST_MPC register type");
    }
}

uint8_t FrameBuilderRt::createFocStartFrame(uint8_t motorId, ST_MPC::CommandId cmd) const 
{
    // Motor ID in 3 MSB, Command in 5 LSB
    return ((motorId & 0x07) << 5) | (static_cast<uint8_t>(cmd) & 0x1F);
}

uint8_t FrameBuilderRt::calculateFocCRC(const std::vector<uint8_t>& focFrame) const 
{
    uint16_t sum = 0;
    for (const auto& byte : focFrame) {
        sum += byte;
    }
    return static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
}

std::vector<uint8_t> FrameBuilderRt::buildFocPayload(uint8_t startFrame, uint8_t payloadLength, 
                                                    std::vector<uint8_t> payload) const 
{
    std::vector<uint8_t> focFrame;
    focFrame.push_back(startFrame);
    focFrame.push_back(payloadLength);
    focFrame.insert(focFrame.end(), payload.begin(), payload.end());
    
    // Calculate and append FOC CRC
    focFrame.push_back(calculateFocCRC(focFrame));
    return focFrame;
}

std::vector<uint8_t> FrameBuilderRt::buildFocReadFrame(uint8_t mscId, ST_MPC::RegisterId regId) 
{
    // Create FOC frame components
    uint8_t startFrame = createFocStartFrame(mscId, ST_MPC::CommandId::GetRegister);
    std::vector<uint8_t> regPayload = {static_cast<uint8_t>(regId)};
    auto focPayload = buildFocPayload(startFrame, 0x01, regPayload);

    // Build RT frame containing FOC payload
    uint8_t commandId = static_cast<uint8_t>(RT::CommandId::FOC_COMMAND);
    uint8_t errorId = static_cast<uint8_t>(RT::ErrorId::NO_ERROR);
    FrameData frame;
    frame.setStartByte(0xAA);
    frame.setTotalSize(16 + focPayload.size() + 1); // header + FOC payload + RT CRC
    frame.setPayloadSize(focPayload.size()+1);      // payload size + CRC
    frame.addPayloadByte(mscId);                    // MSC ID first part
    frame.addPayloadByte(0x0c);                     // Message request ID
    frame.addPayloadByte(0x00);                     // Message response ID
    frame.addPayloadByte(0x63);                     // Conversation ID
    frame.addPayloadByte(0x01);                     // Sender ID
    frame.addPayloadByte(0x01);                     // Number of blocks
    frame.addPayloadByte(0x01);                     // Sequence ID
    frame.addPayloadByte(commandId);                // commandId
    frame.addPayloadByte(errorId);                  // errorId
    frame.addPayloadByte(0x00);                     // Future use 0
    frame.addPayloadByte(0x00);                     // Future use 1
    frame.addPayloadByte(0x00);                     // Future use 2
    frame.addPayloadByte(0xAA);                     // End byte

    // Add FOC payload
    for (const auto& byte : focPayload) {
        frame.addPayloadByte(byte);
    }

    /*
    auto result = frame.complete();
    std::cout << "FrameBuilder::buildFocReadFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;

    return result;
    */

   return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildFocWriteFrame(uint8_t mscId, ST_MPC::RegisterId regId, 
                                                       int32_t value, ST_MPC::RegisterType regType) 
{
    // Create FOC frame components
    uint8_t startFrame = createFocStartFrame(mscId, ST_MPC::CommandId::SetRegister);
    
    // Convert value to bytes according to register type
    auto valueBytes = valueToBytes(value, regType);
    
    // Build FOC payload: register ID followed by value bytes
    std::vector<uint8_t> regPayload = {static_cast<uint8_t>(regId)};
    regPayload.insert(regPayload.end(), valueBytes.begin(), valueBytes.end());
    
    auto focPayload = buildFocPayload(startFrame, regPayload.size(), regPayload);
    
    // Build RT frame containing FOC payload
    uint8_t commandId = static_cast<uint8_t>(RT::CommandId::FOC_COMMAND);
    uint8_t errorId = static_cast<uint8_t>(RT::ErrorId::NO_ERROR);
    FrameData frame;
    frame.setStartByte(0xAA);
    frame.setTotalSize(16 + focPayload.size() + 1); // header + FOC payload + RT CRC
    frame.setPayloadSize(focPayload.size() + 1);    // payload size + CRC
    frame.addPayloadByte(mscId);                    // MSC ID first part
    frame.addPayloadByte(0x0d);                     // Message request ID
    frame.addPayloadByte(0x00);                     // Message response ID
    frame.addPayloadByte(0x63);                     // Conversation ID
    frame.addPayloadByte(0x01);                     // Sender ID
    frame.addPayloadByte(0x01);                     // Number of blocks
    frame.addPayloadByte(0x01);                     // Sequence ID
    frame.addPayloadByte(commandId);                // commandId
    frame.addPayloadByte(errorId);                  // errorId
    frame.addPayloadByte(0x00);                     // Future use 0
    frame.addPayloadByte(0x00);                     // Future use 1
    frame.addPayloadByte(0x00);                     // Future use 2
    frame.addPayloadByte(0xAA);                     // End byte

    // Add FOC payload
    for (const auto& byte : focPayload) {
        frame.addPayloadByte(byte);
    }

    /*
    auto result =  frame.complete();
    std::cout << "FrameBuilder::buildFocWriteFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;

    return result;
    */

    return frame.complete();
}

std::vector<uint8_t> FrameBuilderRt::buildFocExecuteFrame(uint8_t mscId, ST_MPC::ExecuteId execId) 
{
    // Create FOC frame components
    uint8_t startFrame = createFocStartFrame(mscId, ST_MPC::CommandId::Execute);
    std::vector<uint8_t> execPayload = {static_cast<uint8_t>(execId)};
    auto focPayload = buildFocPayload(startFrame, 0x01, execPayload);
    
    // Build RT frame containing FOC payload
    uint8_t commandId = static_cast<uint8_t>(RT::CommandId::FOC_COMMAND);
    uint8_t errorId = static_cast<uint8_t>(RT::ErrorId::NO_ERROR);
    FrameData frame;
    frame.setStartByte(0xAA);
    frame.setTotalSize(16 + focPayload.size() + 1); // header + FOC payload + RT CRC
    frame.setPayloadSize(focPayload.size() + 1);    // payload size + CRC
    frame.addPayloadByte(mscId);                    // MSC ID first part
    frame.addPayloadByte(0x0e);                     // Message request ID
    frame.addPayloadByte(0x00);                     // Message response ID
    frame.addPayloadByte(0x63);                     // Conversation ID
    frame.addPayloadByte(0x01);                     // Sender ID
    frame.addPayloadByte(0x01);                     // Number of blocks
    frame.addPayloadByte(0x01);                     // Sequence ID
    frame.addPayloadByte(commandId);                // commandId
    frame.addPayloadByte(errorId);                  // errorId
    frame.addPayloadByte(0x00);                     // Future use 0
    frame.addPayloadByte(0x00);                     // Future use 1
    frame.addPayloadByte(0x00);                     // Future use 2
    frame.addPayloadByte(0xAA);                     // End byte

    // Add FOC payload
    for (const auto& byte : focPayload) {
        frame.addPayloadByte(byte);
    }

    /*
    auto result = frame.complete();
    std::cout << "FrameBuilderRt::buildFocExecuteFrame()" << std::endl;
    for (auto byte : result) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }   

    std::cout << std::dec << std::endl;

    return result;
    */
    return frame.complete();
}