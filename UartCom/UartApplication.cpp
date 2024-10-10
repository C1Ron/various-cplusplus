#include "UartApplication.h"
#include <iostream>
#include <stdexcept>

UartApplication::UartApplication()
{
    fUart = "";
    fHandle = 0;
}

UartApplication::~UartApplication()
{
    if (fHandle > 0) {
        fUartCom.Close(&fHandle);
    }
}

void UartApplication::ScanArguments(int argc, const char** argv)
{
    bool uartSet = false;  // Track if UART has been set explicitly via -uart

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        std::cout << "Processing argument: " << arg << std::endl;

        if (arg == "-uart") {
            if (i + 1 < argc) {
                fUart = argv[++i];
                std::cout << "UART device set to: " << fUart << std::endl;
                uartSet = true;  // Mark that UART has been explicitly set
            } else {
                throw std::invalid_argument("No UART device specified after -uart.");
            }
        } else if (arg == "-read") {
            fReadMode = true;
        } else if (arg == "-write") {
            if (i + 1 < argc) {
                fMessageWrite = argv[++i];
                std::cout << "Message to write: " << fMessageWrite << std::endl;
            } else {
                throw std::invalid_argument("No message specified after -write.");
            }
        } else if (arg == "-swap") {
            // Handle swap operation if needed
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    // If UART device wasn't set explicitly, use the pre-set read/write devices
    if (fUart.empty() && !uartSet) {
        if (fReadMode) {
            fUart = fUartRead;
        } else {
            fUart = fUartWrite;
        }

        if (fUart.empty()) {
            throw std::invalid_argument("UART device not specified and no virtual UART available.");
        }
    }
}
void UartApplication::Execute()
{
    if (fHandle <= 0) {
        try {
            Open();
        } catch (const std::runtime_error& e) {
            throw std::runtime_error(std::string("Failed to open UART device: ") + e.what());
        }
    }

    if (fReadMode) {
        std::string result = Read(100);  // Adjust the number of bytes to read as needed
        if (!result.empty()) {
            std::cout << "Read message: " << result << std::endl;
        } else {
            std::cout << "No data read from UART" << std::endl;
        }
    } else {
        Write(fMessageWrite);
    }
}

int UartApplication::Open()
{
    std::cout << "Opening UART device: " << fUart << std::endl;
    int result = fUartCom.Open(fUart.c_str(), &fHandle);
    if (result != 0) {
        throw std::runtime_error("Failed to open UART device: " + fUart + " (error code: " + std::to_string(result) + ")");
    }
    return fHandle;
}

void UartApplication::Close()
{
    if (fHandle > 0) {
        std::cout << "Closing UART device: " << fUart << " (handle: " << fHandle << ")" << std::endl;
        fUartCom.Close(&fHandle);
        fHandle = 0;
    }
}

void UartApplication::Write(const std::string& msg)
{
    int bytesWritten = fUartCom.Write(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size(), fHandle);
    if (bytesWritten < 0) {
        std::cerr << "Error writing to UART" << std::endl;
    } else {
        std::cout << "Successfully wrote " << bytesWritten << " bytes to UART" << std::endl;
    }
}

std::string UartApplication::Read(int n_bytes)
{
    uint8_t buffer[4096] = {0};
    int len = fUartCom.Read(buffer, n_bytes, fHandle);
    if (len < 0) {
        std::cerr << "Error reading from UART" << std::endl;
        return "";
    }
    return std::string(reinterpret_cast<char*>(buffer), len);
}

void UartApplication::Help() const
{
    std::cout << "Usage: ./uart-com -uart <device> [-write <message>] [-read]\n";
}

// Setters for the UART read and write devices
void UartApplication::SetUartRead(const std::string& uartRead)
{
    fUartRead = uartRead;  // Set the read UART device
}

void UartApplication::SetUartWrite(const std::string& uartWrite)
{
    fUartWrite = uartWrite;  // Set the write UART device
}
