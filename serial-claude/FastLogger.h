#ifndef FAST_LOGGER_H
#define FAST_LOGGER_H

#include "SerialConnection.h"
#include "CommandHandler.h"
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <fstream>

class FastLogger
{
public:
    FastLogger(SerialConnection& serial, CommandHandler& handler, const std::string& logFile);
    ~FastLogger();

    void addRegister(ST_MPC::RegisterId regId);
    void startLogging();
    void stopLogging();

private:
    void loggingThread();
    void writeHeader();
    uint8_t calculateCRC(const std::vector<uint8_t>& frame);

    SerialConnection& m_serial;
    CommandHandler& m_handler;  
    std::ofstream m_logFile;
    std::vector<ST_MPC::RegisterId> m_registers;
    std::atomic<bool> m_isRunning;
    std::thread m_loggerThread;

    bool m_headerWritten; // Track if header has been written
};

#endif // FAST_LOGGER_H
