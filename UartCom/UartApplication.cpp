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

    // Start processing from argv[1] (skip argv[0], which is the program name)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        std::cout << "Processing argument: " << arg << std::endl;

        if (arg == "-uart") {
            // Ensure that the next argument exists and is not empty
            if (i + 1 < argc && argv[i + 1] != nullptr && std::string(argv[i + 1]).length() > 0) {
                fUart = argv[++i];  // Move to the next argument and assign it to fUart
                std::cout << "UART device set to: " << fUart << std::endl;
                uartSet = true;  // Mark that UART has been explicitly set
            } else {
                throw std::invalid_argument("No UART device specified after -uart.");
            }
        } else if (arg == "-read") {
            fReadMode = true;  // Set read mode
        } else if (arg == "-write") {
            if (i + 1 < argc && argv[i + 1] != nullptr) {
                fMessageWrite = argv[++i];  // Move to the next argument and assign the message to write
                std::cout << "Message to write: " << fMessageWrite << std::endl;
            } else {
                throw std::invalid_argument("No message specified after -write.");
            }
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }

    // Ensure that the UART device is set, either from the command line or VirtualUart
    if (fUart.empty() && !uartSet) {
        if (fReadMode) {
            fUart = fUartRead;  // Use the predefined read UART device
        } else {
            fUart = fUartWrite;  // Use the predefined write UART device
        }

        if (fUart.empty()) {
            throw std::invalid_argument("UART device not specified.");
        }
    }
}

void UartApplication::Execute()
{
    if (fReadMode) {
        std::cout << "Reading from UART device: " << fUart << std::endl;

        int retries = 5;
        while (retries-- > 0) {
            std::string result = Read(100);  // Adjust the number of bytes to read
            if (!result.empty()) {
                std::cout << "Read message: " << result << std::endl;
                break;
            } else {
                std::cerr << "Retrying read... (" << retries << " retries left)" << std::endl;
            }
        }
    } else {
        std::cout << "Writing to UART device: " << fUart << std::endl;
        Write(fMessageWrite);
    }
}

int UartApplication::Open()
{
    return fUartCom.Open(fUart.c_str(), &fHandle);
}

void UartApplication::Write(const std::string& msg)
{
    fUartCom.Write(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size(), fHandle);
}

std::string UartApplication::Read(int n_bytes)
{
    uint8_t buffer[4096] = {0};
    int len = fUartCom.Read(buffer, n_bytes, fHandle);
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
