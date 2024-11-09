#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>

class SignalHandler 
{
public:
    static void setup();
    static bool shouldExit();

private:
    static void handleSignal(int signum);
    static std::atomic<bool> exitFlag;
};

#endif // SIGNAL_HANDLER_H