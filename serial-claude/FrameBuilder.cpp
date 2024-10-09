// FrameBuilder.cpp
#include "FrameBuilder.h"
#include <stdexcept>

std::vector<uint8_t> FrameBuilder::buildSetRegisterFrame(uint8_t motorID, ST_MPC::RegisterId regID, int32_t value, ST_MPC::RegisterType regType) 
{
    std::vector<uint8_t> frame;
    uint8_t frameStart = (motorID << 5) | static_cast<uint8_t>(ST_MPC::CommandId::SetRegister);

    std::vector<uint8_t> regValBytes;
    switch (regType) {
        case ST_MPC::RegisterType::UInt8:
            regValBytes = {static_cast<uint8_t>(value)};
            break;
        case ST_MPC::RegisterType::Int16:
        case ST_MPC::RegisterType::UInt16:
            regValBytes = {static_cast<uint8_t>(value & 0xFF), 
                           static_cast<uint8_t>((value >> 8) & 0xFF)};
            break;
        case ST_MPC::RegisterType::Int32:
        case ST_MPC::RegisterType::UInt32:
            regValBytes = {static_cast<uint8_t>(value & 0xFF), 
                           static_cast<uint8_t>((value >> 8) & 0xFF),
                           static_cast<uint8_t>((value >> 16) & 0xFF),
                           static_cast<uint8_t>((value >> 24) & 0xFF)};
            break;
        default:
            throw std::runtime_error("Unknown register type");
    }

    uint8_t payloadLength = 1 + regValBytes.size();

    frame.push_back(frameStart);
    frame.push_back(payloadLength);
    frame.push_back(static_cast<uint8_t>(regID));
    frame.insert(frame.end(), regValBytes.begin(), regValBytes.end());

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
    frame.push_back(static_cast<uint8_t>(regID));

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
    frame.push_back(static_cast<uint8_t>(execID));

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

    uint8_t crc = calculateCRC(frame);
    frame.push_back(crc);

    return frame;
}

uint8_t FrameBuilder::calculateCRC(const std::vector<uint8_t>& frame) 
{
    uint16_t total = 0;

    for (const auto& byte : frame) {
        total += byte;
    }

    uint8_t lowByte = total & 0x00FF;
    uint8_t highByte = (total & 0xFF00) >> 8;

    uint8_t crc = lowByte + highByte;

    return crc;
}