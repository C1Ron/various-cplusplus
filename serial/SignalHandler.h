// SignalHandler.h
#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>

class SignalHandler {
public:
    static void setup();
    static bool shouldExit();

private:
    static std::atomic<bool> exitFlag;
    static void handleSignal(int sig);
};

#endif // SIGNAL_HANDLER_H