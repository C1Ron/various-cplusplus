#include "LoggerRt.h"
#include "FrameBuilderRt.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::mutex LoggerRt::readMutex;  // Define static mutex

LoggerRt::LoggerRt(SerialConnectionRt& serial, uint8_t mscId, const LogConfig& config)
    : serial(serial), mscId(mscId), config(config) 
{
    // Intentionally empty
}

LoggerRt::~LoggerRt() 
{
    stop();
    if (loggerThread.joinable()) {
        loggerThread.join();
    }
}

void LoggerRt::start() 
{
    if (!running.exchange(true)) {
        // Open file if not already opened
        if (!fileOpened) {
            logFile.open(config.filename, std::ios::out | std::ios::trunc);
            if (!logFile.is_open()) {
                running.store(false);
                throw std::runtime_error("Unable to open log file: " + config.filename);
            }
            fileOpened = true;
        }

        writeHeader();
        loggerThread = std::thread(&LoggerRt::loggingThread, this);
    }
}

void LoggerRt::stop() 
{
    if (running.exchange(false)) {
        if (loggerThread.joinable()) {
            loggerThread.join();
        }
        if (logFile.is_open()) {
            logFile.close();
            fileOpened = false;
        }
    }
}

bool LoggerRt::isRunning() const 
{
    return running.load();
}

bool LoggerRt::removeRegister(const std::string& regName) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RegisterInfo& info) { return info.name == regName; });
    
    if (it == registers.end()) {
        return false;
    }

    registers.erase(it);
    
    // If logging is active, rewrite the header
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

bool LoggerRt::addRegister(const std::string& regName, RT::RegisterId regId, RT::RegisterType type) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    // Check if register already exists
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RegisterInfo& info) { return info.name == regName; });
    
    if (it != registers.end()) {
        return false;
    }

    RegisterInfo info{regId, type, regName};
    registers.push_back(info);
    
    // If logging is active, rewrite the header
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

void LoggerRt::setConfig(const LogConfig& newConfig) 
{
    if (running) {
        throw std::runtime_error("Cannot change config while logger is running");
    }
    config = newConfig;
    fileOpened = false;  // Force file to be reopened with new name
}

const LoggerRt::LogConfig& LoggerRt::getConfig() const 
{
    return config;
}

std::vector<std::string> LoggerRt::getLoggedRegisters() const 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    std::vector<std::string> names;
    names.reserve(registers.size());
    for (const auto& reg : registers) {
        names.push_back(reg.name);
    }
    return names;
}

int32_t LoggerRt::extractValue(const std::vector<uint8_t>& response, RT::RegisterType type)
{
    if (response.size() < 17) { // 16 byte header + at least 1 byte payload
        throw std::runtime_error("Invalid response size");
    }

    switch (type) {
        case RT::RegisterType::UInt8:
            return response[16];  // First byte after header
            
        case RT::RegisterType::Int16:
            return static_cast<int16_t>(response[16] | (response[17] << 8));
            
        case RT::RegisterType::UInt16:
            return static_cast<uint16_t>(response[16] | (response[17] << 8));
            
        case RT::RegisterType::Int32:
            return static_cast<int32_t>(response[16] | (response[17] << 8) |
                (response[18] << 16) | (response[19] << 24));
            
        case RT::RegisterType::UInt32:
            return static_cast<uint32_t>(response[16] | (response[17] << 8) |
                (response[18] << 16) | (response[19] << 24));
            
        case RT::RegisterType::Float: {
            if (response.size() < 20) { // Need 4 bytes for float
                throw std::runtime_error("Invalid response size for float");
            }
            // Convert float to int32 representation
            uint32_t bits = static_cast<uint32_t>(response[16] | (response[17] << 8) |
                                                 (response[18] << 16) | (response[19] << 24));
            float value = *reinterpret_cast<float*>(&bits);
            // Convert to fixed-point representation (multiply by 1000 to keep 3 decimal places)
            return static_cast<int32_t>(value * 1000.0f);
        }
            
        default:
            throw std::runtime_error("Unsupported register type for value extraction");
    }
}

void LoggerRt::loggingThread() 
{
    FrameBuilderRt frameBuilder;
    
    while (running.load()) {
        try {
            auto timestamp = std::chrono::system_clock::now();
            std::map<std::string, int32_t> values;

            // Take a snapshot of registers with minimal lock time
            std::vector<RegisterInfo> regs;
            {
                std::lock_guard<std::mutex> lock(registersMutex);
                regs = registers;
            }

            if (regs.empty()) {
                std::this_thread::sleep_for(config.sampleInterval);
                continue;
            }

            // Read all registers without locking
            for (const auto& reg : regs) {
                try {
                    auto frame = frameBuilder.buildReadFrame(mscId, reg.id);
                    
                    std::lock_guard<std::mutex> lock(readMutex);
                    serial.sendFrame(frame);
                    auto response = serial.readFrame();

                    if (response.size() >= 17) { // Minimum valid response size
                        values[reg.name] = extractValue(response, reg.type);
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Error reading " << reg.name << ": " << e.what() << std::endl;
                }
            }

            // Write values if we got any
            if (!values.empty()) {
                writeLogLine(timestamp, values);
            }

            std::this_thread::sleep_for(config.sampleInterval);
        }
        catch (const std::exception& e) {
            std::cerr << "Logger error: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void LoggerRt::writeHeader() 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    logFile.seekp(0);
    logFile.clear();

    if (registers.empty()) {
        return;
    }

    if (config.useTimestamp) {
        logFile << "Timestamp";
    }

    for (const auto& reg : registers) {
        if (config.useTimestamp || &reg != &registers.front()) {
            logFile << ",";
        }
        logFile << reg.name;
    }
    logFile << "\n";
    logFile.flush();
}

void LoggerRt::writeLogLine(const std::chrono::system_clock::time_point& timestamp,
                           const std::map<std::string, int32_t>& values) 
{
    if (config.useTimestamp) {
        auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
            timestamp.time_since_epoch()).count();
        logFile << ts;
    }

    std::lock_guard<std::mutex> lock(registersMutex);
    for (const auto& reg : registers) {
        if (config.useTimestamp || &reg != &registers.front()) {
            logFile << ",";
        }
        auto it = values.find(reg.name);
        if (it != values.end()) {
            if (reg.type == RT::RegisterType::Float) {
                // Convert back from fixed-point to decimal
                logFile << std::fixed << std::setprecision(3) << (it->second / 1000.0);
            } else {
                logFile << it->second;
            }
        }
    }
    logFile << "\n";
    logFile.flush();
}