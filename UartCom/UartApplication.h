#ifndef UARTAPPLICATION_H
#define UARTAPPLICATION_H

#include "UartCom.h"
#include <string>
#include <functional>

class UartApplication
{
public:
    UartApplication();
    ~UartApplication();

    // Configuration
    void SetUartDevice(const std::string& uartDevice);
    std::string GetUartDevice() const;

    // Command-line argument parsing
    void ParseArguments(int argc, const char** argv);

    // UART operations
    void OpenUart();
    void CloseUart();
    void WriteToUart(const std::string& message);
    std::string ReadFromUart(int numBytes);

    // Execution
    void Execute();

    // Help
    static void ShowUsage();

private:
    std::string m_uartDevice;
    std::string m_messageToWrite;
    bool m_isReadMode;
    UartCom m_uartCom;
    int m_uartHandle;

    // Helper methods
    void ValidateArguments() const;
    void EnsureUartIsOpen();
};

#endif // UARTAPPLICATION_H