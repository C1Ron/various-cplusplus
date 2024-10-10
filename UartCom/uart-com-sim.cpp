#include "UartCom.h"
#include "VirtualUart2.h"
#include <iostream>

int main() {
    VirtualUart vuart;
    vuart.Run();       // Start the virtual UART
    vuart.Generate();  // Scan for UART pairs

    // Display virtual UART devices
    std::cout << "UART Read: " << vuart.GetUartRead() << std::endl;
    std::cout << "UART Write: " << vuart.GetUartWrite() << std::endl;

    // Write to one virtual UART
    UartCom uart;
    int handleWrite = 0;
    if (uart.Open(vuart.GetUartWrite().c_str(), &handleWrite) == 0) {
        std::string message = "Hello Virtual UART!";
        uart.Write(reinterpret_cast<const uint8_t*>(message.c_str()), message.size(), handleWrite);
        uart.Close(&handleWrite);
    }

    // Read from the other virtual UART
    int handleRead = 0;
    if (uart.Open(vuart.GetUartRead().c_str(), &handleRead) == 0) {
        uint8_t buffer[100] = {0};
        uart.Read(buffer, sizeof(buffer), handleRead);
        std::cout << "Read message: " << buffer << std::endl;
        uart.Close(&handleRead);
    }

    return 0;
}
