#include "SignalHandler.h"
#include <iostream>

std::atomic<bool> SignalHandler::exitFlag{false};
SignalHandler::ShutdownCallback SignalHandler::shutdownCallback;

void SignalHandler::setup(ShutdownCallback callback) 
{
    shutdownCallback = std::move(callback);
    std::signal(SIGINT, handleSignal);  // Ctrl+C
    std::signal(SIGTERM, handleSignal); // Termination request
    std::signal(SIGTSTP, handleSignal); // Ctrl+Z
}

bool SignalHandler::shouldExit(bool setExit) 
{
    if (setExit) {
        exitFlag.store(true, std::memory_order_release);
    }
    return exitFlag.load(std::memory_order_acquire);
}

void SignalHandler::reset() 
{
    exitFlag.store(false, std::memory_order_release);
}

void SignalHandler::handleSignal(int signum) 
{
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    exitFlag.store(true, std::memory_order_release);
    if (shutdownCallback) {
        shutdownCallback();
    }
}