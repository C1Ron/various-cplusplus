#include "SignalHandler.h"
#include <csignal>
#include <iostream>

std::atomic<bool> SignalHandler::exitFlag{false};

void SignalHandler::setup() {
    std::signal(SIGINT, handleSignal);  // Ctrl+C
    std::signal(SIGTERM, handleSignal); // Termination request
}

bool SignalHandler::shouldExit() {
    return exitFlag.load();
}

void SignalHandler::handleSignal([[maybe_unused]] int sig) {
    // We can optionally print the signal number if we want to use it
    // std::cout << "\nReceived signal " << sig << std::endl;
    exitFlag.store(true);
}