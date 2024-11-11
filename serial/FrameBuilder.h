// FrameBuilder.h
#ifndef FRAME_BUILDER_H
#define FRAME_BUILDER_H

#include "StMpcDefinitions.h"
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>

class FrameBuilder 
{
public:
    class FrameError : public std::runtime_error 
    {
    public:
        explicit FrameError(const std::string& msg) : std::runtime_error(msg) 
        {
            // intentionally empty
        }
    };

    std::vector<uint8_t> buildSetFrame(uint8_t motorId, 
                                             ST_MPC::RegisterId regId,
                                             int32_t value, 
                                             ST_MPC::RegisterType regType);
                                             
    std::vector<uint8_t> buildGetFrame(uint8_t motorId,
                                             ST_MPC::RegisterId regId);
                                             
    std::vector<uint8_t> buildExecuteFrame(uint8_t motorId,
                                         ST_MPC::ExecuteId execId);
                                         
    std::vector<uint8_t> buildRampFrame(uint8_t motorId,
                                             int32_t finalSpeed,
                                             uint16_t duration);
    
    std::vector<uint8_t> buildCurrentFrame(uint8_t motorId,
                                              int16_t IqRef,
                                              int16_t IdRef);

private:
    class FrameData 
    {
    public:
        void setStartByte(uint8_t byte) 
        { 
            frame.push_back(byte); 
        }
        void setPayloadLength(uint8_t length) 
        { 
            frame.push_back(length); 
        }
        void addPayloadByte(uint8_t byte) 
        { 
            frame.push_back(byte); 
        }
        void addPayloadBytes(const std::vector<uint8_t>& bytes) 
        {
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