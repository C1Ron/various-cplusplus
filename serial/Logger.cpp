#include "Logger.h"
#include "FrameBuilder.h"
#include <iostream>
#include <iomanip>
#include <sstream>

std::mutex Logger::readMutex;  // Define static mutex

Logger::Logger(SerialConnection& serial, const LogConfig& config)
    : serial(serial), config(config) 
{
    // Intentionally empty
}

Logger::~Logger() 
{
    stop();
    if (loggerThread.joinable()) {
        loggerThread.join();
    }
}

void Logger::start() 
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
        loggerThread = std::thread(&Logger::loggingThread, this);
    }
}

void Logger::stop() 
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

bool Logger::isRunning() const 
{
    return running.load();
}

bool Logger::removeRegister(const std::string& regName) 
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

bool Logger::addRegister(const std::string& regName, ST_MPC::RegisterId regId, ST_MPC::RegisterType type) 
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

void Logger::setConfig(const LogConfig& newConfig) 
{
    if (running) {
        throw std::runtime_error("Cannot change config while logger is running");
    }
    config = newConfig;
    fileOpened = false;  // Force file to be reopened with new name
}

const Logger::LogConfig& Logger::getConfig() const 
{
    return config;
}

std::vector<std::string> Logger::getLoggedRegisters() const 
{
    std::lock_guard<std::mutex> lock(registersMutex);
    std::vector<std::string> names;
    names.reserve(registers.size());
    for (const auto& reg : registers) {
        names.push_back(reg.name);
    }
    return names;
}

int32_t Logger::extractValue(const std::vector<uint8_t>& response, ST_MPC::RegisterType type)
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

void Logger::loggingThread() 
{
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
                    FrameBuilder frameBuilder;
                    auto frame = frameBuilder.buildGetFrame(1, reg.id);
                    serial.sendFrame(frame);
                    auto response = serial.readFrame();

                    if (response.size() >= 4 && response[0] == 0xF0) {
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

void Logger::writeHeader() 
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

void Logger::writeLogLine(const std::chrono::system_clock::time_point& timestamp,
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
            logFile << it->second;
        }
    }
    logFile << "\n";
    logFile.flush();
}

int32_t Logger::readRegisterValue(const RegisterInfo& reg) 
{
    std::lock_guard<std::mutex> lock(readMutex);

    try {
        FrameBuilder frameBuilder;
        auto frame = frameBuilder.buildGetFrame(1, reg.id);
        serial.sendFrame(frame);
        auto response = serial.readFrame();

        if (response.size() < 4) {
            throw std::runtime_error("Invalid response size");
        }

        if (response[0] != 0xF0) {
            throw std::runtime_error("Register read failed");
        }

        uint8_t payloadLength = response[1];
        
        if (response.size() != static_cast<size_t>(payloadLength) + 3) {
            throw std::runtime_error("Invalid response length");
        }

        int32_t value = 0;
        switch (reg.type) {
            case ST_MPC::RegisterType::UInt8:
                if (payloadLength != 1) throw std::runtime_error("Invalid payload length for UInt8");
                value = response[2];
                break;

            case ST_MPC::RegisterType::Int16:
                if (payloadLength != 2) throw std::runtime_error("Invalid payload length for Int16");
                value = static_cast<int16_t>(
                    response[2] | 
                    (response[3] << 8)
                );
                break;

            case ST_MPC::RegisterType::UInt16:
                if (payloadLength != 2) throw std::runtime_error("Invalid payload length for UInt16");
                value = static_cast<uint16_t>(
                    response[2] | 
                    (response[3] << 8)
                );
                break;

            case ST_MPC::RegisterType::Int32:
                if (payloadLength != 4) throw std::runtime_error("Invalid payload length for Int32");
                value = static_cast<int32_t>(
                    response[2] | 
                    (response[3] << 8) |
                    (response[4] << 16) | 
                    (response[5] << 24)
                );
                break;

            case ST_MPC::RegisterType::UInt32:
                if (payloadLength != 4) throw std::runtime_error("Invalid payload length for UInt32");
                value = static_cast<uint32_t>(
                    response[2] | 
                    (response[3] << 8) |
                    (response[4] << 16) | 
                    (response[5] << 24)
                );
                break;

            case ST_MPC::RegisterType::CharPtr:
                if (payloadLength < 1) throw std::runtime_error("Invalid payload length for CharPtr");
                std::string str(response.begin() + 2, response.end());
                std::istringstream iss(str);
                iss >> value;
                break;
        }
        
        return value;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to read register: ") + e.what());
    }
}