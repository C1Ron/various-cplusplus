#ifndef UARTAPPLICATION_H
#define UARTAPPLICATION_H

#include "UartCom.h"
#include <string>

class UartApplication
{
public:
    UartApplication();
    ~UartApplication();

    void ScanArguments(int argc, const char** argv);
    void Execute();
    int Open();
    void Write(const std::string& msg);
    std::string Read(int n_bytes);
    void Help() const;

    // Setters for UART Read and Write devices
    void SetUartRead(const std::string& uartRead);   // Setter for UART read device
    void SetUartWrite(const std::string& uartWrite); // Setter for UART write device

private:
    std::string fUart;       // The current UART device used for operations
    std::string fMessageWrite;  // Message to write
    bool fReadMode = false;   // Whether the application is in read mode
    UartCom fUartCom;         // UART communication object
    int fHandle = 0;          // File descriptor for the UART device

    std::string fUartRead;    // UART device for reading (from VirtualUart)
    std::string fUartWrite;   // UART device for writing (from VirtualUart)
};

#endif // UARTAPPLICATION_H
