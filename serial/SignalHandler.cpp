#include "SignalHandler.h"
#include <csignal>
#include <iostream>

std::atomic<bool> SignalHandler::exitFlag{false};

void SignalHandler::setup() 
{
    std::signal(SIGINT, handleSignal);  // Ctrl+C
    std::signal(SIGTERM, handleSignal); // Termination request
}

bool SignalHandler::shouldExit() 
{
    return exitFlag.load();
}

void SignalHandler::handleSignal([[maybe_unused]] int signum) 
{
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    exitFlag.store(true);
}