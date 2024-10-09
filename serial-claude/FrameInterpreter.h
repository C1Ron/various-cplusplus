#ifndef FRAME_INTERPRETER_H
#define FRAME_INTERPRETER_H

#include <vector>
#include <string>
#include <cstdint>

class FrameInterpreter 
{
public:
    std::string interpretResponse(const std::vector<uint8_t>& response);
    void printResponse(const std::vector<uint8_t>& response) const;  // Make sure this line is present

private:
    std::string interpretSuccessResponse(const std::vector<uint8_t>& response) const;
    std::string interpretErrorResponse(const std::vector<uint8_t>& response) const;
    std::string byteToHexString(uint8_t byte) const;
};

#endif // FRAME_INTERPRETER_H