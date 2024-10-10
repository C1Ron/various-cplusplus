#include <iostream>
#include "UartApplication.h"
#include "VirtualUart.h"

void showUsage()
{
    std::cout << "Usage: ./uart-cli [-uart <device>] [-read | -write <message>] [-swap]\n";
    std::cout << "If -uart is not specified, virtual UART devices will be used.\n";
}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        showUsage();
        return -1;
    }

    std::unique_ptr<VirtualUart> vUart;
    UartApplication app;
    bool useVirtualUart = true;
    
    // First, scan for -uart argument
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-uart" && i + 1 < argc) {
            useVirtualUart = false;
            break;
        }
    }

    if (useVirtualUart) {
        // Initialize Virtual UART
        vUart = std::make_unique<VirtualUart>();
        vUart->Run();
        vUart->Generate();  // Generate the virtual UART devices

        std::string uartReadDevice = vUart->GetUartRead();
        std::string uartWriteDevice = vUart->GetUartWrite();

        std::cout << "Using Virtual UART devices:" << std::endl;
        std::cout << "UART Read Device: " << uartReadDevice << std::endl;
        std::cout << "UART Write Device: " << uartWriteDevice << std::endl;

        app.SetUartRead(uartReadDevice);
        app.SetUartWrite(uartWriteDevice);
    } else {
        std::cout << "Using specified UART device." << std::endl;
    }

    try {
        app.ScanArguments(argc, argv);
        app.Execute();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}