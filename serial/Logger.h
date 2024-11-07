// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include "SerialConnection.h"
#include "StMpcDefinitions.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class Logger {
public:
    struct LogConfig {
        std::string filename;
        std::chrono::milliseconds sampleInterval{100};  // Default 100ms
        size_t bufferSize{1024};
        bool useTimestamp{true};
    };

    Logger(SerialConnection& serial, const LogConfig& config);
    ~Logger();

    bool addRegister(const std::string& regName, ST_MPC::RegisterId regId);
    bool removeRegister(const std::string& regName);
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
    
    // Get current configuration
    const LogConfig& getConfig() const { return config; }
    
    // Get list of currently logged registers
    std::vector<std::string> getLoggedRegisters() const;

private:
    struct RegisterInfo {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
        std::string name;
    };

    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string regName;
        int32_t value;
    };

    void loggingThread();
    void writeHeader();
    void writeEntry(const LogEntry& entry);
    int32_t readRegisterValue(const RegisterInfo& reg);

    SerialConnection& serial;
    LogConfig config;
    std::ofstream logFile;
    
    std::atomic<bool> running{false};
    std::thread loggerThread;
    
    mutable std::mutex registersMutex;
    std::vector<RegisterInfo> registers;

    mutable std::mutex logMutex;
    std::vector<LogEntry> logBuffer;
};

#endif // LOGGER_H