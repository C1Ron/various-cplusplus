#include "UartApplication.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <csignal>

std::atomic<bool> g_running(true);

void signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    g_running = false;
}

std::pair<std::string, std::string> getVirtualUartDevices() {
    std::ifstream readFile("virtual-uart1.txt");
    std::ifstream writeFile("virtual-uart2.txt");
    std::string readDevice, writeDevice;
    
    if (readFile.is_open() && writeFile.is_open()) {
        std::getline(readFile, readDevice);
        std::getline(writeFile, writeDevice);
    }
    
    return {readDevice, writeDevice};
}

int main(int argc, const char** argv) {
    signal(SIGINT, signalHandler);

    try {
        std::vector<std::string> args(argv, argv + argc);
        bool isReadMode = false;
        bool isVirtualUart = false;

        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "-read") {
                isReadMode = true;
            }
            if (args[i] == "-uart" && i + 1 < args.size() && args[i + 1] == "virtual") {
                isVirtualUart = true;
                break;
            }
        }

        std::string uartDevice;
        if (isVirtualUart) {
            auto [readDevice, writeDevice] = getVirtualUartDevices();
            if (readDevice.empty() || writeDevice.empty()) {
                std::cerr << "Virtual UART devices not found. Please run virtual-uart-manager first." << std::endl;
                return 1;
            }
            std::cout << "Using Virtual UART. Read device: " << readDevice 
                      << ", Write device: " << writeDevice << std::endl;
            uartDevice = isReadMode ? readDevice : writeDevice;
        } else {
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i] == "-uart" && i + 1 < args.size()) {
                    uartDevice = args[i + 1];
                    break;
                }
            }
        }

        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "-uart") {
                args[i + 1] = uartDevice;
                break;
            }
        }

        std::vector<const char*> cargs;
        for (const auto& arg : args) {
            cargs.push_back(arg.c_str());
        }

        while (g_running) {
            UartApplication app;
            app.ParseArguments(cargs.size(), cargs.data());
            app.Execute();

            if (!isReadMode) {
                std::cout << "Enter message to write (or 'exit' to quit): ";
                std::string message;
                std::getline(std::cin, message);
                
                if (message == "exit") {
                    g_running = false;
                } else if (!message.empty()) {
                    for (size_t i = 1; i < args.size(); ++i) {
                        if (args[i] == "-write") {
                            args[i + 1] = message;
                            cargs[i + 1] = args[i + 1].c_str();
                            break;
                        }
                    }
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "UART communication terminated." << std::endl;
    return 0;
}