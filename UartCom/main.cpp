#include <iostream>
#include "UartApplication.h"
#include "VirtualUart.h"

void showUsage()
{
    std::cout << "Usage: ./uart-cli -uart <device> [-read | -write <message>] [-swap]\n";
}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        showUsage();
        return -1;
    }

    // Initialize Virtual UART
    VirtualUart vUart;
    vUart.Run();
    vUart.Generate();  // Generate the virtual UART devices

    std::string uartReadDevice = vUart.GetUartRead();
    std::string uartWriteDevice = vUart.GetUartWrite();

    // Debug output
    std::cout << "UART Read Device: " << uartReadDevice << std::endl;
    std::cout << "UART Write Device: " << uartWriteDevice << std::endl;

    // Instantiate UartApplication
    UartApplication app;
    app.SetUartRead(uartReadDevice);    // Set the read UART device
    app.SetUartWrite(uartWriteDevice);  // Set the write UART device

    // Handle command-line arguments
    app.ScanArguments(argc, argv);
    app.Execute();

    return 0;
}
