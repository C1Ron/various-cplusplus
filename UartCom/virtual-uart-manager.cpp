#include "VirtualUart.h"
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> g_running(true);

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    g_running = false;
}

int main() {
    signal(SIGINT, signalHandler);

    VirtualUart vUart;
    vUart.Run();
    vUart.Generate();

    std::cout << "Virtual UART set up. Read device: " << vUart.GetUartRead() 
              << ", Write device: " << vUart.GetUartWrite() << std::endl;

    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Virtual UART manager shutting down." << std::endl;
    return 0;
}