#include "VirtualMsc.h"

VirtualMsc::VirtualMsc() 
{    
    registers[0x04] = {int32_t(0), true};  // SpeedRef (int32)
    registers[0x08] = {int16_t(0), false};  // TorqueRef (int16)
}

std::vector<uint8_t> VirtualMsc::processFrame(const std::vector<uint8_t>& frame) 
{
    if (frame.size() < 4) {
        return createErrorResponse(0x01); // Invalid frame size
    }

    uint8_t startFrame = frame[0];
    uint8_t payloadLength = frame[1];
    // uint8_t motorId = (startFrame >> 5) & 0x07;
    uint8_t commandId = startFrame & 0x1F;

    if (frame.size() != static_cast<uint8_t>(payloadLength + 3)) {
        return createErrorResponse(0x02); // Payload length mismatch
    }

    uint8_t receivedCRC = frame.back();
    std::vector<uint8_t> frameWithoutCRC(frame.begin(), frame.end() - 1);
    uint8_t calculatedCRC = calculateCRC(frameWithoutCRC);

    if (receivedCRC != calculatedCRC) {
        return createErrorResponse(0x03); // CRC mismatch
    }

    std::vector<uint8_t> payload(frame.begin() + 2, frame.end() - 1);

    std::vector<uint8_t> response;
    switch (commandId) {
        case SET_COMMAND:
            response = handleSetCommand(payload);
            break;
        case GET_COMMAND:
            response = handleGetCommand(payload);
            break;
        case EXECUTE_COMMAND:
        case RAMP_COMMAND:
            response = createErrorResponse(0x04); // Unsupported command
            break;
        default:
            response = createErrorResponse(0x05); // Invalid command
            break;
    }

    // debugPrint(response, "Sending");
    return response;
}

uint8_t VirtualMsc::calculateCRC(const std::vector<uint8_t>& frame) 
{
    uint16_t total = 0;
    for (const auto& byte : frame) {
        total += byte;
    }
    uint8_t lowByte = total & 0x00FF;
    uint8_t highByte = (total & 0xFF00) >> 8;
    return lowByte + highByte;
}

std::vector<uint8_t> VirtualMsc::createSuccessResponse(const std::vector<uint8_t>& payload) 
{
    std::vector<uint8_t> response;
    response.push_back(SUCCESS_FRAME_ACK);
    response.push_back(payload.size());
    response.insert(response.end(), payload.begin(), payload.end());
    uint8_t crc = calculateCRC(response);
    response.push_back(crc);
    return response;
}

std::vector<uint8_t> VirtualMsc::createErrorResponse(uint8_t errorCode) 
{
    std::vector<uint8_t> response = {FAILURE_FRAME_ACK, 0x01, errorCode};
    uint8_t crc = calculateCRC(response);
    response.push_back(crc);
    return response;
}

std::vector<uint8_t> VirtualMsc::handleSetCommand(const std::vector<uint8_t>& payload) 
{
    if (payload.size() < 3) {  // At least register address + 2 bytes of value
        return createErrorResponse(0x06); // Invalid payload size for SET command
    }

    uint8_t regAddress = payload[0];
    auto it = registers.find(regAddress);
    if (it == registers.end()) {
        return createErrorResponse(0x07); // Invalid register address
    }

    if (it->second.is32bit) {
        if (payload.size() != 5) {  // register address + 4 bytes of value
            return createErrorResponse(0x06); // Invalid payload size for SET command
        }
        int32_t value;
        std::memcpy(&value, payload.data() + 1, sizeof(int32_t));
        it->second.value = value;
    } else {
        if (payload.size() != 3) {  // register address + 2 bytes of value
            return createErrorResponse(0x06); // Invalid payload size for SET command
        }
        int16_t value;
        std::memcpy(&value, payload.data() + 1, sizeof(int16_t));
        it->second.value = value;
    }
    
    std::vector<uint8_t> response = {SUCCESS_FRAME_ACK, 0x00};
    uint8_t crc = calculateCRC(response);
    response.push_back(crc);
    return response;
}

std::vector<uint8_t> VirtualMsc::handleGetCommand(const std::vector<uint8_t>& payload) 
{
    if (payload.size() != 1) {
        return createErrorResponse(0x08); // Invalid payload size for GET command
    }

    uint8_t regAddress = payload[0];
    auto it = registers.find(regAddress);
    if (it == registers.end()) {
        return createErrorResponse(0x09); // Invalid register address
    }

    std::vector<uint8_t> response;
    response.push_back(SUCCESS_FRAME_ACK);

    if (it->second.is32bit) {
        response.push_back(0x04);  // Payload length for int32
        int32_t value = std::get<int32_t>(it->second.value);
        response.insert(response.end(), reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(int32_t));
    } else {
        response.push_back(0x02);  // Payload length for int16
        int16_t value = std::get<int16_t>(it->second.value);
        response.insert(response.end(), reinterpret_cast<uint8_t*>(&value), reinterpret_cast<uint8_t*>(&value) + sizeof(int16_t));
    }

    // Increment the value after reading
    if (it->second.is32bit) {
        std::get<int32_t>(it->second.value)++;
    } else {
        std::get<int16_t>(it->second.value)++;
    }

    uint8_t crc = calculateCRC(response);
    response.push_back(crc);
    return response;
}

void VirtualMsc::debugPrint(const std::vector<uint8_t>& frame, const std::string& label) 
{
    std::cout << label << " frame (" << frame.size() << " bytes): ";
    for (const auto& byte : frame) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}