// FrameBuilder.h
#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include <vector>
#include <cstdint>
#include "StMpcDefinitions.h"

class FrameBuilder 
{
public:
    std::vector<uint8_t> buildSetRegisterFrame(uint8_t motorID, ST_MPC::RegisterId regID, int32_t value, ST_MPC::RegisterType regType);
    std::vector<uint8_t> buildGetRegisterFrame(uint8_t motorID, ST_MPC::RegisterId regID);
    std::vector<uint8_t> buildExecuteFrame(uint8_t motorID, ST_MPC::ExecuteId execID);
    std::vector<uint8_t> buildExecuteRampFrame(uint8_t motorID, int32_t finalSpeed, uint16_t duration);

private:
    uint8_t calculateCRC(const std::vector<uint8_t>& frame);
};

#endif // FRAME_BUILDER_H