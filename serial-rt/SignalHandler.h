#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>
#include <csignal>
#include <functional>
#include <memory>

class SignalHandler 
{
public:
    using ShutdownCallback = std::function<void()>;
    static void setup(ShutdownCallback callback);
    static bool shouldExit(bool setExit = false);          // optional parameter to set exit flag
    static void reset();

private:
    static void handleSignal(int signum);
    static std::atomic<bool> exitFlag;
    static ShutdownCallback shutdownCallback;
};

#endif // SIGNAL_HANDLER_H