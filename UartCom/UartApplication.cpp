#include "UartApplication.h"
#include <iostream>
#include <stdexcept>

UartApplication::UartApplication()
    : m_isReadMode(false), m_uartHandle(-1)
{
}

UartApplication::~UartApplication()
{
    CloseUart();
}

void UartApplication::SetUartDevice(const std::string& uartDevice)
{
    m_uartDevice = uartDevice;
}

std::string UartApplication::GetUartDevice() const
{
    return m_uartDevice;
}

void UartApplication::ParseArguments(int argc, const char** argv)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-uart") {
            if (++i < argc) m_uartDevice = argv[i];
            else throw std::invalid_argument("No UART device specified after -uart.");
        } else if (arg == "-read") {
            m_isReadMode = true;
        } else if (arg == "-write") {
            if (++i < argc) m_messageToWrite = argv[i];
            else throw std::invalid_argument("No message specified after -write.");
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
    ValidateArguments();
}

void UartApplication::OpenUart()
{
    if (m_uartHandle >= 0) return; // Already open
    
    int result = m_uartCom.Open(m_uartDevice.c_str(), &m_uartHandle);
    if (result != 0) {
        throw std::runtime_error("Failed to open UART device: " + m_uartDevice);
    }
}

void UartApplication::CloseUart()
{
    if (m_uartHandle >= 0) {
        m_uartCom.Close(&m_uartHandle);
        m_uartHandle = -1;
    }
}

void UartApplication::WriteToUart(const std::string& message)
{
    EnsureUartIsOpen();
    int bytesWritten = m_uartCom.Write(reinterpret_cast<const uint8_t*>(message.c_str()), message.size(), m_uartHandle);
    if (bytesWritten < 0) {
        throw std::runtime_error("Error writing to UART");
    }
    std::cout << "Successfully wrote " << bytesWritten << " bytes to UART" << std::endl;
}

std::string UartApplication::ReadFromUart(int numBytes)
{
    EnsureUartIsOpen();
    std::vector<uint8_t> buffer(numBytes);
    int bytesRead = m_uartCom.Read(buffer.data(), numBytes, m_uartHandle);
    if (bytesRead < 0) {
        throw std::runtime_error("Error reading from UART");
    }
    return std::string(buffer.begin(), buffer.begin() + bytesRead);
}

void UartApplication::Execute()
{
    try {
        OpenUart();
        if (m_isReadMode) {
            std::string result = ReadFromUart(100); // Adjust buffer size as needed
            std::cout << "Read from UART: " << result << std::endl;
        } else {
            WriteToUart(m_messageToWrite);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
    }
}

void UartApplication::ShowUsage()
{
    std::cout << "Usage: ./uart-cli -uart <device> [-read | -write <message>]" << std::endl;
}

void UartApplication::ValidateArguments() const
{
    if (m_uartDevice.empty()) {
        throw std::invalid_argument("UART device not specified. Use -uart <device>.");
    }
    if (!m_isReadMode && m_messageToWrite.empty()) {
        throw std::invalid_argument("Either -read or -write <message> must be specified.");
    }
}

void UartApplication::EnsureUartIsOpen()
{
    if (m_uartHandle < 0) {
        OpenUart();
    }
}