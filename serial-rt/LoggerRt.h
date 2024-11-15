// LoggerRt.h
#ifndef LOGGER_RT_H
#define LOGGER_RT_H

#include "SerialConnectionRt.h"
#include "RtDefinitions.h"
#include "StMpcDefinitions.h"
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <variant>

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

    struct RtRegisterInfo 
    {
        RT::RegisterId id;
        RT::RegisterType type;
        std::string name;
        bool isFoc;  // false for RT, true for FOC
    };

    LoggerRt(SerialConnectionRt& serial, uint8_t mscId, const LogConfig& config);
    ~LoggerRt();
    
    void start();
    void stop();
    bool isRunning() const;
    
    // RT register handling
    bool addRtRegister(const std::string& regName, RT::RegisterId regId, RT::RegisterType type);
    bool removeRtRegister(const std::string& regName);
    
    // FOC register handling
    bool addFocRegister(const std::string& regName, ST_MPC::RegisterId regId, ST_MPC::RegisterType type);
    bool removeFocRegister(const std::string& regName);
    
    void setConfig(const LogConfig& newConfig);
    const LogConfig& getConfig() const;
    std::vector<std::string> getLoggedRegisters() const;

private:
    SerialConnectionRt& serial;
    uint8_t mscId;
    LogConfig config;
    std::ofstream logFile;
    bool fileOpened{false};

    std::atomic<bool> running{false};
    std::thread loggerThread;
    std::vector<RtRegisterInfo> registers;
    
    static std::mutex readMutex;
    mutable std::mutex registersMutex;

    int32_t extractRtValue(const std::vector<uint8_t>& response, RT::RegisterType type);
    int32_t extractFocValue(const std::vector<uint8_t>& response, ST_MPC::RegisterType type);

    void loggingThread();
    void writeHeader();
    void writeLogLine(const std::chrono::system_clock::time_point& timestamp, 
                     const std::map<std::string, int32_t>& values);
};

#endif // LOGGER_RT_H