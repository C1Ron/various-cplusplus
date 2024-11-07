#include "Logger.h"
#include "FrameBuilder.h"
#include <iomanip>
#include <iostream>
#include <sstream>

Logger::Logger(SerialConnection& serial, const LogConfig& config)
    : serial(serial), config(config) {
    
    logFile.open(config.filename, std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        throw std::runtime_error("Unable to open log file: " + config.filename);
    }
}

Logger::~Logger() {
    stop();
    if (loggerThread.joinable()) {
        loggerThread.join();
    }
}

bool Logger::addRegister(const std::string& regName, ST_MPC::RegisterId regId) {
    std::lock_guard<std::mutex> lock(registersMutex);
    
    // Check if register already exists
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RegisterInfo& info) { return info.name == regName; });
    
    if (it != registers.end()) {
        return false;
    }

    RegisterInfo info{regId, ST_MPC::RegisterType::Int32, regName}; // Default to Int32
    registers.push_back(info);
    
    // If logging is active, update the header
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

bool Logger::removeRegister(const std::string& regName) {
    std::lock_guard<std::mutex> lock(registersMutex);
    
    auto it = std::find_if(registers.begin(), registers.end(),
        [&regName](const RegisterInfo& info) { return info.name == regName; });
    
    if (it == registers.end()) {
        return false;
    }

    registers.erase(it);
    
    // If logging is active, update the header
    if (running.load()) {
        writeHeader();
    }
    
    return true;
}

void Logger::start() {
    if (!running.exchange(true)) {
        writeHeader();
        loggerThread = std::thread(&Logger::loggingThread, this);
    }
}

void Logger::stop() {
    running.store(false);
    if (loggerThread.joinable()) {
        loggerThread.join();
    }
}

std::vector<std::string> Logger::getLoggedRegisters() const {
    std::lock_guard<std::mutex> lock(registersMutex);
    std::vector<std::string> names;
    names.reserve(registers.size());
    for (const auto& reg : registers) {
        names.push_back(reg.name);
    }
    return names;
}

void Logger::writeHeader() {
    std::lock_guard<std::mutex> lock(logMutex);
    logFile.seekp(0);
    logFile.clear();
    
    if (config.useTimestamp) {
        logFile << "Timestamp,";
    }
    
    std::lock_guard<std::mutex> regLock(registersMutex);
    for (size_t i = 0; i < registers.size(); ++i) {
        logFile << registers[i].name;
        if (i < registers.size() - 1) {
            logFile << ",";
        }
    }
    logFile << "\n";
    logFile.flush();
}

void Logger::writeEntry(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (config.useTimestamp) {
        auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            entry.timestamp.time_since_epoch()).count();
        logFile << timestamp << ",";
    }
    
    logFile << entry.value << "\n";
    logFile.flush();
}

int32_t Logger::readRegisterValue(const RegisterInfo& reg) {
    FrameBuilder frameBuilder;
    auto frame = frameBuilder.buildGetRegisterFrame(1, reg.id);
    serial.sendFrame(frame);
    auto response = serial.readFrame();
    
    // Basic response validation
    if (response.size() < 4) {
        throw std::runtime_error("Invalid response size");
    }
    
    // Extract value based on type
    int32_t value = 0;
    switch (reg.type) {
        case ST_MPC::RegisterType::UInt8:
            value = response[2];
            break;
        case ST_MPC::RegisterType::Int16:
            value = static_cast<int16_t>(response[2] | (response[3] << 8));
            break;
        case ST_MPC::RegisterType::UInt16:
            value = static_cast<uint16_t>(response[2] | (response[3] << 8));
            break;
        case ST_MPC::RegisterType::Int32:
            value = static_cast<int32_t>(response[2] | (response[3] << 8) |
                                       (response[4] << 16) | (response[5] << 24));
            break;
        case ST_MPC::RegisterType::UInt32:
            value = static_cast<uint32_t>(response[2] | (response[3] << 8) |
                                        (response[4] << 16) | (response[5] << 24));
            break;
    }
    
    return value;
}

void Logger::loggingThread() {
    while (running.load()) {
        auto timestamp = std::chrono::system_clock::now();
        
        std::vector<RegisterInfo> currentRegisters;
        {
            std::lock_guard<std::mutex> lock(registersMutex);
            currentRegisters = registers;  // Copy for thread safety
        }
        
        for (const auto& reg : currentRegisters) {
            try {
                int32_t value = readRegisterValue(reg);
                LogEntry entry{timestamp, reg.name, value};
                writeEntry(entry);
            } catch (const std::exception& e) {
                std::cerr << "Error reading register " << reg.name << ": " << e.what() << std::endl;
            }
        }
        
        std::this_thread::sleep_for(config.sampleInterval);
    }
}