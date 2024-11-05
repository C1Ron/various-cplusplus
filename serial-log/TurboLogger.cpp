#include "TurboLogger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

TurboLogger::TurboLogger(SerialConnection* serial, CommandHandler& handler, const std::string& logFile)
    : m_serial(serial), 
      m_handler(handler), 
      m_isRunning(false),
      m_stopRequested(false),
      m_headerWritten(false),
      m_buffer(BUFFER_SIZE)
{
    m_logFile.open(logFile, std::ios::out | std::ios::trunc);
    if (!m_logFile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + logFile);
    }
}

TurboLogger::~TurboLogger()
{
    stopLogging();
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

bool TurboLogger::addRegister(ST_MPC::RegisterId regId) 
{
    std::lock_guard<std::mutex> lock(m_registerMutex);
    if (std::find(m_registers.begin(), m_registers.end(), regId) == m_registers.end()) {
        m_registers.push_back(regId);
        return true;
    }
    return false;
}

bool TurboLogger::removeRegister(ST_MPC::RegisterId regId) 
{
    std::lock_guard<std::mutex> lock(m_registerMutex);
    auto it = std::find(m_registers.begin(), m_registers.end(), regId);
    if (it != m_registers.end()) {
        m_registers.erase(it);
        return true;
    }
    return false;
}

void TurboLogger::startLogging()
{
    if (!m_isRunning.exchange(true)) {
        m_stopRequested.store(false);
        m_loggerThread = std::thread(&TurboLogger::loggingThread, this);
    }
}

void TurboLogger::stopLogging()
{
    m_stopRequested.store(true);
    m_isRunning.store(false);
    if (m_loggerThread.joinable()) {
        m_loggerThread.join();
    }
}

void TurboLogger::writeHeader()
{
    if (!m_headerWritten) {
        m_logFile << "time";
        for (const auto& regId : m_registers) {
            m_logFile << ",reg-" << static_cast<int>(regId);
        }
        m_logFile << "\n";
        m_logFile.flush();
        m_headerWritten = true;
    }
}

void TurboLogger::loggingThread()
{
    writeHeader();

    std::vector<uint8_t> frame(4);
    size_t bufferIndex = 0;
    
    while (!m_stopRequested.load(std::memory_order_relaxed)) {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count();

        int printed = snprintf(m_buffer.data() + bufferIndex, BUFFER_SIZE - bufferIndex, "%lu", timestamp);
        if (printed < 0 || static_cast<size_t>(printed) >= BUFFER_SIZE - bufferIndex) {
            std::cerr << "Buffer overflow when writing timestamp" << std::endl;
            break;
        }
        bufferIndex += printed;

        std::lock_guard<std::mutex> lock(m_registerMutex);
        for (const auto& regId : m_registers) {
            if (bufferIndex >= BUFFER_SIZE - 1) {
                std::cerr << "Buffer full, flushing early" << std::endl;
                m_logFile.write(m_buffer.data(), bufferIndex);
                bufferIndex = 0;
            }

            frame[0] = 0x02;
            frame[1] = 0x01;
            frame[2] = static_cast<uint8_t>(regId);
            frame[3] = calculateCRC(frame);
            
            m_serial->sendFrame(frame);
            auto response = m_serial->readFrame();

            m_buffer[bufferIndex++] = ',';
            if (response.size() >= 4 && response.size() == static_cast<size_t>(response[1] + 3)) {
                if (response.back() != calculateCRC(response)) {
                    const char* error = "ERROR";
                    size_t errorLen = strlen(error);
                    if (bufferIndex + errorLen < BUFFER_SIZE) {
                        std::memcpy(m_buffer.data() + bufferIndex, error, errorLen);
                        bufferIndex += errorLen;
                    }
                    continue;
                }

                auto regTypeIt = m_handler.getRegisterTypeMap().find(regId);
                if (regTypeIt == m_handler.getRegisterTypeMap().end()) {
                    const char* error = "ERROR";
                    size_t errorLen = strlen(error);
                    if (bufferIndex + errorLen < BUFFER_SIZE) {
                        std::memcpy(m_buffer.data() + bufferIndex, error, errorLen);
                        bufferIndex += errorLen;
                    }
                    continue;
                }

                ST_MPC::RegisterType regType = regTypeIt->second;
                int32_t value = 0;

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
                    case ST_MPC::RegisterType::UInt32:
                        value = static_cast<int32_t>((response[2]) | (response[3] << 8) |
                                                     (response[4] << 16) | (response[5] << 24));
                        break;
                    default:
                        const char* error = "ERROR";
                        size_t errorLen = strlen(error);
                        if (bufferIndex + errorLen < BUFFER_SIZE) {
                            std::memcpy(m_buffer.data() + bufferIndex, error, errorLen);
                            bufferIndex += errorLen;
                        }
                        continue;
                }

                printed = snprintf(m_buffer.data() + bufferIndex, BUFFER_SIZE - bufferIndex, "%d", value);
                if (printed < 0 || static_cast<size_t>(printed) >= BUFFER_SIZE - bufferIndex) {
                    std::cerr << "Buffer overflow when writing value" << std::endl;
                    break;
                }
                bufferIndex += printed;
            } else {
                const char* error = "ERROR";
                size_t errorLen = strlen(error);
                if (bufferIndex + errorLen < BUFFER_SIZE) {
                    std::memcpy(m_buffer.data() + bufferIndex, error, errorLen);
                    bufferIndex += errorLen;
                }
            }
        }

        if (bufferIndex >= BUFFER_SIZE - 1) {
            m_logFile.write(m_buffer.data(), bufferIndex);
            bufferIndex = 0;
        } else {
            m_buffer[bufferIndex++] = '\n';
        }

        if (bufferIndex > BUFFER_SIZE / 2) {
            m_logFile.write(m_buffer.data(), bufferIndex);
            bufferIndex = 0;
        }
        // Check if stop is requested
        if (m_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
    }

    if (bufferIndex > 0) {
        m_logFile.write(m_buffer.data(), bufferIndex);
    }
    std::cout << "Logging thread exit" << std::endl;
}

uint8_t TurboLogger::calculateCRC(const std::vector<uint8_t>& frame)
{
    uint16_t sum = 0;
    for (size_t i = 0; i < frame.size() - 1; ++i) {
        sum += frame[i];
    }
    return static_cast<uint8_t>((sum & 0xFF) + (sum >> 8));
}