#include "SignalHandler.h"
#include <csignal>
#include <iostream>

std::atomic<bool> SignalHandler::shutdownFlag(false);

void SignalHandler::registerHandler()
{
    std::signal(SIGINT, SignalHandler::signalHandler);
}

bool SignalHandler::shouldShutdown()
{
    return shutdownFlag.load();
}

void SignalHandler::signalHandler(int signum)
{
    std::cout << "\nInterrupt signal (" << signum << ") received." << std::endl;
    shutdownFlag.store(true);
}
