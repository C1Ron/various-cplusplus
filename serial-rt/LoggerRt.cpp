// LoggerRt.cpp
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

// Basic operations remain the same
void LoggerRt::start() 
{
    if (!running.exchange(true)) {
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

// New register handling methods
bool LoggerRt::addRtRegister(const std::string& regName, RT::RegisterId regId, RT::RegisterType type) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RtRegisterInfo& info) { return info.name == regName; });
    
    if (it != registers.end()) {
        return false;
    }

    RtRegisterInfo info{regId, type, regName, false};  // false = RT register
    registers.push_back(info);
    
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

bool LoggerRt::addFocRegister(const std::string& regName, ST_MPC::RegisterId regId, ST_MPC::RegisterType type) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RtRegisterInfo& info) { return info.name == regName; });
    
    if (it != registers.end()) {
        return false;
    }

    // Store FOC register using RT types but mark as FOC
    RtRegisterInfo info{
        static_cast<RT::RegisterId>(regId),
        static_cast<RT::RegisterType>(type),
        regName,
        true  // true = FOC register
    };
    registers.push_back(info);
    
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

bool LoggerRt::removeRtRegister(const std::string& regName) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RtRegisterInfo& info) { 
            return info.name == regName && !info.isFoc; 
        });
    
    if (it == registers.end()) {
        return false;
    }

    registers.erase(it);
    
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

bool LoggerRt::removeFocRegister(const std::string& regName) 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RtRegisterInfo& info) { 
            return info.name == regName && info.isFoc; 
        });
    
    if (it == registers.end()) {
        return false;
    }

    registers.erase(it);
    
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

// Value extraction methods
int32_t LoggerRt::extractRtValue(const std::vector<uint8_t>& response, RT::RegisterType type)
{
    if (response.size() < 17) {
        throw std::runtime_error("Invalid response size");
    }

    switch (type) {
        case RT::RegisterType::UInt8:
            return response[16];
            
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
            if (response.size() < 20) {
                throw std::runtime_error("Invalid response size for float");
            }
            // Skip register ID byte (response[16]) and start from response[17]
            uint32_t bits = static_cast<uint32_t>(response[17] | (response[18] << 8) |
                                                (response[19] << 16) | (response[20] << 24));
            float value = *reinterpret_cast<float*>(&bits);
            return static_cast<int32_t>(value * 1000.0f);
        }
            
        default:
            throw std::runtime_error("Unsupported register type for value extraction");
    }
}

int32_t LoggerRt::extractFocValue(const std::vector<uint8_t>& response, ST_MPC::RegisterType type)
{
    // FOC response format: [mscId, errorCode, ack, focPayloadLength, focPayload, focCrc]
    if (response.size() < 20) {  // Minimum size for FOC response
        throw std::runtime_error("Invalid FOC response size");
    }

    // FOC payload starts at index 20 (16 + 4)
    const uint8_t* payload = response.data() + 20;
    
    switch (type) {
        case ST_MPC::RegisterType::UInt8:
            return payload[0];
            
        case ST_MPC::RegisterType::Int16:
            return static_cast<int16_t>(payload[0] | (payload[1] << 8));
            
        case ST_MPC::RegisterType::UInt16:
            return static_cast<uint16_t>(payload[0] | (payload[1] << 8));
            
        case ST_MPC::RegisterType::Int32:
            return static_cast<int32_t>(payload[0] | (payload[1] << 8) |
                (payload[2] << 16) | (payload[3] << 24));
            
        case ST_MPC::RegisterType::UInt32:
            return static_cast<uint32_t>(payload[0] | (payload[1] << 8) |
                (payload[2] << 16) | (payload[3] << 24));
            
        default:
            throw std::runtime_error("Unsupported FOC register type for value extraction");
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
            std::vector<RtRegisterInfo> regs;
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
                    std::vector<uint8_t> frame;
                    if (reg.isFoc) {
                        // Build FOC read frame
                        frame = frameBuilder.buildFocReadFrame(mscId, 
                            static_cast<ST_MPC::RegisterId>(reg.id));
                    } else {
                        // Build RT read frame
                        frame = frameBuilder.buildReadFrame(mscId, reg.id);
                    }
                    
                    std::lock_guard<std::mutex> lock(readMutex);
                    serial.sendFrame(frame);
                    auto response = serial.readFrame();

                    if (response.size() >= 17) {  // Minimum valid response size
                        if (reg.isFoc) {
                            values[reg.name] = extractFocValue(response, 
                                static_cast<ST_MPC::RegisterType>(reg.type));
                        } else {
                            values[reg.name] = extractRtValue(response, reg.type);
                        }
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
        // Include type (RT or FOC) in the register name
        std::string prefix = reg.isFoc ? "FOC: " : "RT:  ";
        names.push_back(prefix + reg.name);
    }
    return names;
}