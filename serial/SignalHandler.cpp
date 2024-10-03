#include "SignalHandler.h"
#include <iostream>

// Global pointer to the SerialConnection object
SerialConnection* serialConnectionPtr = nullptr;

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    if (serialConnectionPtr) {
        // Properly delete the object and set the pointer to nullptr
        delete serialConnectionPtr;
        serialConnectionPtr = nullptr;
    }
    exit(signum);
}

// Register the signal handler and set the SerialConnection instance
void registerHandler(SerialConnection* connection)
{
    serialConnectionPtr = connection;
    signal(SIGINT, signalHandler);  // Handle CTRL+C
    signal(SIGTERM, signalHandler); // Handle termination
    signal(SIGTSTP, signalHandler); // Handle CTRL+Z (optional)
}
