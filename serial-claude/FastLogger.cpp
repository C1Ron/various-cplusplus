#include "FastLogger.h"
#include <iostream>
#include <iomanip>

FastLogger::FastLogger(SerialConnection& serial, CommandHandler& handler, const std::string& logFile)
    : m_serial(serial), m_handler(handler), m_isRunning(false)
{
    // Open the file in trunc mode to clear existing content
    m_logFile.open(logFile, std::ios::out | std::ios::trunc);
    if (!m_logFile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + logFile);
    }

    // Write the header immediately
    writeHeader();
}

FastLogger::~FastLogger()
{
    stopLogging();
    if (m_loggerThread.joinable()) {
        m_loggerThread.join();
    }
}

void FastLogger::addRegister(ST_MPC::RegisterId regId)
{
    m_registers.push_back(regId);
}

void FastLogger::startLogging()
{
    if (!m_isRunning.exchange(true)) {  // Start only if not already running
        writeHeader();  // Write header when logging starts
        m_loggerThread = std::thread(&FastLogger::loggingThread, this);
    }
}

void FastLogger::stopLogging()
{
    m_isRunning.store(false);
    if (m_loggerThread.joinable()) {
        m_loggerThread.join();
    }
}

void FastLogger::writeHeader()
{
    if (m_registers.empty()) {
        std::cerr << "No registers to log." << std::endl;
        return;  // Don't write a header if there are no registers
    }
    
    m_logFile << "time"; // Start with the time column
    for (const auto& regId : m_registers) {
        m_logFile << ", reg-" << static_cast<int>(regId); // Create header entries for each register
    }
    m_logFile << "\n"; // End the header line
    m_logFile.flush(); // Ensure the header is written to the file
}

void FastLogger::loggingThread()
{
    while (m_isRunning.load()) {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count();

        m_logFile << timestamp;

        for (const auto& regId : m_registers) {
            std::vector<uint8_t> frame = {0x02, 0x01, static_cast<uint8_t>(regId), 0x00}; // Request frame
            frame[3] = calculateCRC(frame);
            m_serial.sendFrame(frame);

            auto response = m_serial.readFrame();
            if (response.size() >= 4 && static_cast<std::vector<uint8_t>::size_type>(response[1] + 3) == response.size()) {
                if (response.back() != calculateCRC(response)) {
                    std::cerr << "Invalid CRC in response" << std::endl;
                    m_logFile << ", ";  // Keep output aligned by writing an empty field
                    continue;
                }

                auto regTypeIt = m_handler.getRegisterTypeMap().find(regId);
                if (regTypeIt == m_handler.getRegisterTypeMap().end()) {
                    std::cerr << "Unknown register type for register ID: " << static_cast<int>(regId) << std::endl;
                    m_logFile << ", ";  // Empty field for missing register
                    continue;
                }

                ST_MPC::RegisterType regType = regTypeIt->second;
                int32_t value = 0;

                // Interpret the payload based on the register type
                switch (regType) {
                    case ST_MPC::RegisterType::UInt8:
                        value = static_cast<uint8_t>(response[2]);
                        break;
                    case ST_MPC::RegisterType::Int16:
                        value = static_cast<int16_t>((response[2]) | (response[3] << 8));
                        break;
                    case ST_MPC::RegisterType::UInt16:
                        value = static_cast<uint16_t>((response[2]) | (response[3] << 8));
                        break;
                    case ST_MPC::RegisterType::Int32:
                        value = static_cast<int32_t>((response[2]) | (response[3] << 8) |
                                                     (response[4] << 16) | (response[5] << 24));
                        break;
                    case ST_MPC::RegisterType::UInt32:
                        value = static_cast<uint32_t>((response[2]) | (response[3] << 8) |
                                                      (response[4] << 16) | (response[5] << 24));
                        break;
                    default:
                        std::cerr << "Unsupported register type for register ID: " << static_cast<int>(regId) << std::endl;
                        m_logFile << ", ";  // Empty field for unsupported type
                        continue;
                }

                m_logFile << "," << value;
            } else {
                std::cerr << "Unexpected frame size: " << response.size() << ", Expected: " 
                          << (static_cast<std::vector<uint8_t>::size_type>(response[1]) + 3) << std::endl;
                m_logFile << ", ";  // Empty field for invalid frame
            }
        }

        m_logFile << "\n";
        m_logFile.flush();  // Ensure each log entry is written immediately
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Adjust interval as needed
    }
}

uint8_t FastLogger::calculateCRC(const std::vector<uint8_t>& frame)
{
    uint16_t sum = 0;
    for (size_t i = 0; i < frame.size() - 1; ++i) {
        sum += frame[i];
    }
    return static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
}
