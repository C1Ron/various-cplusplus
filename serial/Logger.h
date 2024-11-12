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

    bool addRegister(const std::string& regName, ST_MPC::RegisterId regId, ST_MPC::RegisterType type);
    bool removeRegister(const std::string& regName);
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
    std::vector<std::string> getLoggedRegisters() const;
    void setConfig(const LogConfig& newConfig);         // Setter for config
    const LogConfig& getConfig() const;                 // Getter for config

private:
    struct RegisterInfo 
    {
        ST_MPC::RegisterId id;
        ST_MPC::RegisterType type;
        std::string name;
    };

    void loggingThread();
    void writeHeader();
    void writeLogLine(const std::chrono::system_clock::time_point& timestamp, 
                     const std::map<std::string, int32_t>& values);
    int32_t readRegisterValue(const RegisterInfo& reg);

    SerialConnection& serial;
    LogConfig config;
    std::ofstream logFile;
    bool fileOpened{false};

    std::atomic<bool> running{false};
    std::thread loggerThread;
    
    mutable std::mutex registersMutex;
    std::vector<RegisterInfo> registers;  // Using vector to maintain order
    
    static std::mutex readMutex;  // Static mutex for coordinating reads
    
    int32_t extractValue(const std::vector<uint8_t>& response, ST_MPC::RegisterType type) 
    {
            switch (type) {
                case ST_MPC::RegisterType::UInt8:
                    return response[2];
                case ST_MPC::RegisterType::Int16:
                    return static_cast<int16_t>(response[2] | (response[3] << 8));
                case ST_MPC::RegisterType::UInt16:
                    return static_cast<uint16_t>(response[2] | (response[3] << 8));
                case ST_MPC::RegisterType::Int32:
                    return static_cast<int32_t>(response[2] | (response[3] << 8) |
                        (response[4] << 16) | (response[5] << 24));
                case ST_MPC::RegisterType::UInt32:
                    return static_cast<uint32_t>(response[2] | (response[3] << 8) |
                        (response[4] << 16) | (response[5] << 24));
                default:
                    throw std::runtime_error("Unknown register type");
            }
        }
};

#endif // LOGGER_H