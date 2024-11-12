#ifndef LOGGER_H
#define LOGGER_H

#include "SerialConnection.h"
#include "StMpcDefinitions.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <map>
#include <string>
#include <thread>
#include <vector>

class Logger 
{
public:
    struct LogConfig 
    {
        std::string filename;
        std::chrono::milliseconds sampleInterval{100};
        size_t bufferSize{1024};
        bool useTimestamp{true};
    };

    Logger(SerialConnection& serial, const LogConfig& config);
    ~Logger();
    
    void start();
    void stop();

    bool isRunning() const;
    bool removeRegister(const std::string& regName);
    bool addRegister(const std::string& regName, ST_MPC::RegisterId regId, ST_MPC::RegisterType type);

    void setConfig(const LogConfig& newConfig);
    const LogConfig& getConfig() const;

    std::vector<std::string> getLoggedRegisters() const;

private:
    struct RegisterInfo 
    {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
        std::string name;
    };

    SerialConnection& serial;

    LogConfig config;
    std::ofstream logFile;
    bool fileOpened{false};

    std::atomic<bool> running{false};       // Atomic flag for logging thread
    std::thread loggerThread;               // Thread for logging
    
    std::vector<RegisterInfo> registers;    // Using vector to maintain order
    
    static std::mutex readMutex;            // Static mutex for coordinating reads
    mutable std::mutex registersMutex;      // Mutex for protecting registers vector

    int32_t extractValue(const std::vector<uint8_t>& response, ST_MPC::RegisterType type);

    void loggingThread();
    void writeHeader();
    void writeLogLine(const std::chrono::system_clock::time_point& timestamp, 
                     const std::map<std::string, int32_t>& values);

    int32_t readRegisterValue(const RegisterInfo& reg);

};

#endif // LOGGER_H