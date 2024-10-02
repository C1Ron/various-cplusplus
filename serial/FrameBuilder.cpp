#include "FrameBuilder.h"

std::vector<uint8_t> FrameBuilder::buildSetRegisterFrame(uint8_t motorID, ST_MPC::RegisterId regID, const std::vector<uint8_t>& regVal) 
{
    std::vector<uint8_t> frame;

    uint8_t frameStart = (motorID << 5) | static_cast<uint8_t>(ST_MPC::CommandId::SetRegister);
    uint8_t payloadLength = 1 + regVal.size();

    frame.push_back(frameStart);
    frame.push_back(payloadLength);
    frame.push_back(static_cast<uint8_t>(regID)); // Use the enum for RegisterId
    frame.insert(frame.end(), regVal.begin(), regVal.end()); // Register value

    uint8_t crc = calculateCRC(frame);
    frame.push_back(crc);

    return frame;
}

std::vector<uint8_t> FrameBuilder::buildGetRegisterFrame(uint8_t motorID, ST_MPC::RegisterId regID) 
{
    std::vector<uint8_t> frame;
    uint8_t frameStart = (motorID << 5) | static_cast<uint8_t>(ST_MPC::CommandId::GetRegister);

    frame.push_back(frameStart);
    frame.push_back(0x01); // Payload length = 1
    frame.push_back(static_cast<uint8_t>(regID)); // Use the enum for RegisterId

    uint8_t crc = calculateCRC(frame);
    frame.push_back(crc);

    return frame;
}

std::vector<uint8_t> FrameBuilder::buildExecuteFrame(uint8_t motorID, ST_MPC::ExecuteId execID) 
{
    std::vector<uint8_t> frame;

    uint8_t frameStart = (motorID << 5) | static_cast<uint8_t>(ST_MPC::CommandId::Execute);
    frame.push_back(frameStart);
    frame.push_back(0x01); // Payload length = 1
    frame.push_back(static_cast<uint8_t>(execID)); // Use the enum for ExecuteId

    uint8_t crc = calculateCRC(frame);
    frame.push_back(crc);

    return frame;
}

std::vector<uint8_t> FrameBuilder::buildExecuteRampFrame(uint8_t motorID, int32_t finalSpeed, uint16_t duration) 
{
    std::vector<uint8_t> frame;

    uint8_t frameStart = (motorID << 5) | static_cast<uint8_t>(ST_MPC::CommandId::ExecuteRamp);
    frame.push_back(frameStart);

    frame.push_back(0x06); // Payload length = 6

    frame.push_back(static_cast<uint8_t>(finalSpeed & 0xFF));
    frame.push_back(static_cast<uint8_t>((finalSpeed >> 8) & 0xFF));
    frame.push_back(static_cast<uint8_t>((finalSpeed >> 16) & 0xFF));
    frame.push_back(static_cast<uint8_t>((finalSpeed >> 24) & 0xFF));
    frame.push_back(static_cast<uint8_t>(duration & 0xFF));
    frame.push_back(static_cast<uint8_t>((duration >> 8) & 0xFF));

    // CRC Calculation and append it to the frame
    uint8_t crc = calculateCRC(frame);
    frame.push_back(crc);

    return frame;
}

uint8_t FrameBuilder::calculateCRC(const std::vector<uint8_t>& frame) 
{
    uint16_t total = 0;

    // Sum all bytes in the frame
    for (const auto& byte : frame) {
        total += byte;
    }

    uint8_t lowByte = total & 0x00FF;
    uint8_t highByte = (total & 0xFF00) >> 8;

    uint8_t crc = lowByte + highByte;

    return crc; // Append this to the frame
}