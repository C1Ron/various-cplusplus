// FrameBuilderRt.h
#ifndef FRAME_BUILDER_RT_H
#define FRAME_BUILDER_RT_H

#include "RtDefinitions.h"
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
    std::vector<uint8_t> buildRampFrame(uint8_t mscId, int32_t finalSpeed, 
                                       uint16_t duration);

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

    std::vector<uint8_t> valueToBytes(int32_t value, RT::RegisterType type);
    void validateValue(int32_t value, RT::RegisterType type);
};

#endif // FRAME_BUILDER_RT_H