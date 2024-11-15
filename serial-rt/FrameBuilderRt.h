// FrameBuilderRt.h
#ifndef FRAME_BUILDER_RT_H
#define FRAME_BUILDER_RT_H

#include "RtDefinitions.h"
#include "StMpcDefinitions.h"
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>

class FrameBuilderRt 
{
public:
    class FrameError : public std::runtime_error 
    {
    public:
        explicit FrameError(const std::string& msg) : std::runtime_error(msg) {}
    };

    std::vector<uint8_t> buildReadFrame(uint8_t mscId, RT::RegisterId regId);
    std::vector<uint8_t> buildWriteFrame(uint8_t mscId, RT::RegisterId regId, 
                                        int32_t value, RT::RegisterType regType);
    std::vector<uint8_t> buildExecuteFrame(uint8_t mscId, RT::ExecuteId execId);
    std::vector<uint8_t> buildFocReadFrame(uint8_t mscId, ST_MPC::RegisterId regId);
    std::vector<uint8_t> buildFocWriteFrame(uint8_t mscId, ST_MPC::RegisterId regId, 
                                           int32_t value, ST_MPC::RegisterType regType);
    std::vector<uint8_t> buildFocExecuteFrame(uint8_t mscId, ST_MPC::ExecuteId execId);


private:
    class FrameData 
    {
    public:
        void setStartByte(uint8_t byte) { frame.push_back(byte); }
        void setTotalSize(uint8_t size) { frame.push_back(size); }
        void setPayloadSize(uint8_t size) { frame.push_back(size); }
        void addPayloadByte(uint8_t byte) { frame.push_back(byte); }
        void addPayloadBytes(const std::vector<uint8_t>& bytes);
        std::vector<uint8_t> complete();

    private:
        std::vector<uint8_t> frame;
        uint8_t calculateCRC() const;
    };
    std::vector<uint8_t> focPayload;

    std::vector<uint8_t> valueToBytes(int32_t value, RT::RegisterType type);
    std::vector<uint8_t> valueToBytes(int32_t value, ST_MPC::RegisterType type);
    void validateValue(int32_t value, RT::RegisterType type);
    uint8_t createFocStartFrame(uint8_t motorId, ST_MPC::CommandId cmd) const;
    std::vector<uint8_t> buildFocPayload(uint8_t startFrame, uint8_t payloadLength, 
                                        std::vector<uint8_t> payload) const;
    uint8_t calculateFocCRC(const std::vector<uint8_t>& focFrame) const;




};

#endif // FRAME_BUILDER_RT_H