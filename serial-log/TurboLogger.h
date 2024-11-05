// TurboLogger.h
#ifndef TURBO_LOGGER_H
#define TURBO_LOGGER_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include <atomic>
#include <thread>
#include <vector>
#include <fstream>
#include <mutex>

class TurboLogger
{
public:
    TurboLogger(SerialConnection* serial, CommandHandler& handler, const std::string& logFile);
    ~TurboLogger();

    TurboLogger(const TurboLogger&) = delete;
    TurboLogger& operator=(const TurboLogger&) = delete;
    TurboLogger(TurboLogger&&) = delete;
    TurboLogger& operator=(TurboLogger&&) = delete;

    bool addRegister(ST_MPC::RegisterId regId);
    bool removeRegister(ST_MPC::RegisterId regId);
    void startLogging();
    void stopLogging();

private:
    void writeHeader();
    void loggingThread();
    uint8_t calculateCRC(const std::vector<uint8_t>& frame);

    SerialConnection* m_serial;
    CommandHandler& m_handler;
    std::ofstream m_logFile;
    std::vector<ST_MPC::RegisterId> m_registers;
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_stopRequested;
    std::thread m_loggerThread;
    std::mutex m_registerMutex;
    bool m_headerWritten;

    static constexpr size_t BUFFER_SIZE = 4096;
    std::vector<char> m_buffer;
};

#endif // TURBO_LOGGER_H