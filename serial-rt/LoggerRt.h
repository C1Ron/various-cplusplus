#ifndef LOGGER_RT_H
#define LOGGER_RT_H

#include "SerialConnection.h"
#include "RtDefinitions.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <map>
#include <string>
#include <thread>
#include <vector>

class LoggerRt 
{
public:
    struct LogConfig 
    {
        std::string filename;
        std::chrono::milliseconds sampleInterval{100};
        size_t bufferSize{1024};
        bool useTimestamp{true};
    };

    LoggerRt(SerialConnection& serial, uint8_t mscId, const LogConfig& config);
    ~LoggerRt();
    
    void start();
    void stop();
    bool isRunning() const;
    bool removeRegister(const std::string& regName);
    bool addRegister(const std::string& regName, RT::RegisterId regId, RT::RegisterType type);
    void setConfig(const LogConfig& newConfig);
    const LogConfig& getConfig() const;
    std::vector<std::string> getLoggedRegisters() const;

private:
    struct RegisterInfo 
    {
        RT::RegisterId id;
        RT::RegisterType type;
        std::string name;
    };

    SerialConnection& serial;
    uint8_t mscId;
    LogConfig config;
    std::ofstream logFile;
    bool fileOpened{false};

    std::atomic<bool> running{false};
    std::thread loggerThread;
    std::vector<RegisterInfo> registers;
    
    static std::mutex readMutex;
    mutable std::mutex registersMutex;

    int32_t extractValue(const std::vector<uint8_t>& response, RT::RegisterType type);

    void loggingThread();
    void writeHeader();
    void writeLogLine(const std::chrono::system_clock::time_point& timestamp, 
                     const std::map<std::string, int32_t>& values);

    int32_t readRegisterValue(const RegisterInfo& reg);
};

#endif // LOGGER_RT_H