#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <csignal>
#include "SerialConnection.h"

void registerHandler(SerialConnection* connection);
void signalHandler(int signum);

#endif // SIGNAL_HANDLER_H
