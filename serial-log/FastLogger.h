#ifndef FAST_LOGGER_H
#define FAST_LOGGER_H

#include "SerialConnection.h"
#include "StMpcDefinitions.h"
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <fstream>
#include <unordered_map>

class FastLogger {
public:
    FastLogger(SerialConnection& serial, 
              const std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType>& regTypeMap,
              const std::string& logFile);
    ~FastLogger();

    bool addRegister(ST_MPC::RegisterId regId);
    bool removeRegister(ST_MPC::RegisterId regId);
    void startLogging();
    void stopLogging();

private:
    void loggingThread();
    void writeHeader();
    uint8_t calculateCRC(const std::vector<uint8_t>& frame);

    SerialConnection& m_serial;
    const std::unordered_map<ST_MPC::RegisterId, ST_MPC::RegisterType>& m_regTypeMap;
    std::ofstream m_logFile;
    std::vector<ST_MPC::RegisterId> m_registers;
    std::atomic<bool> m_isRunning;
    std::thread m_loggerThread;
    bool m_headerWritten;
};

#endif // FAST_LOGGER_H