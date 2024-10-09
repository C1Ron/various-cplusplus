#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>

class SignalHandler
{
public:
    static void registerHandler();
    static bool shouldShutdown();

private:
    static void signalHandler(int signum);
    static std::atomic<bool> shutdownFlag;
};

#endif // SIGNAL_HANDLER_H
