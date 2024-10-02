// FrameInterpreter.h
#ifndef FRAME_INTERPRETER_H
#define FRAME_INTERPRETER_H

#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <iostream>

class FrameInterpreter 
{
public:
    std::string frameDecoder(const std::vector<uint8_t>& response);
    std::string interpretResponse(const std::vector<uint8_t>& response);
    void printResponse(const std::vector<uint8_t>& response);
};

#endif // FRAME_INTERPRETER_H
