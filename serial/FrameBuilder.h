// FrameBuilder.h
#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "StMpcDefinitions.h"
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>

class FrameBuilder {
public:
    class FrameError : public std::runtime_error {
    public:
        explicit FrameError(const std::string& msg) : std::runtime_error(msg) {}
    };

    std::vector<uint8_t> buildSetRegisterFrame(uint8_t motorId, 
                                             ST_MPC::RegisterId regId,
                                             int32_t value, 
                                             ST_MPC::RegisterType regType);
                                             
    std::vector<uint8_t> buildGetRegisterFrame(uint8_t motorId,
                                             ST_MPC::RegisterId regId);
                                             
    std::vector<uint8_t> buildExecuteFrame(uint8_t motorId,
                                         ST_MPC::ExecuteId execId);
                                         
    std::vector<uint8_t> buildExecuteRampFrame(uint8_t motorId,
                                             int32_t finalSpeed,
                                             uint16_t duration);

private:
    class FrameData {
    public:
        void setStartByte(uint8_t byte) { frame.push_back(byte); }
        void setPayloadLength(uint8_t length) { frame.push_back(length); }
        void addPayloadByte(uint8_t byte) { frame.push_back(byte); }
        void addPayloadBytes(const std::vector<uint8_t>& bytes) {
            frame.insert(frame.end(), bytes.begin(), bytes.end());
        }
        std::vector<uint8_t> complete();

    private:
        std::vector<uint8_t> frame;
        uint8_t calculateCRC() const;
    };

    std::vector<uint8_t> valueToBytes(int32_t value, ST_MPC::RegisterType type);
    void validateValue(int32_t value, ST_MPC::RegisterType type);
};

#endif // FRAME_BUILDER_H