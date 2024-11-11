#include "CommandLine.h"
#include "SerialConnection.h"
#include "SignalHandler.h"
#include "FastLogger.h"
#include "TurboLogger.h"
#include "CommandHandler.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return 1;
    }

    const std::string port = argv[1];

    try {
        SerialConnection serial(port, 115200);
        CommandHandler handler(serial);
        
       // Create logger and attach it to handler
        auto logger = std::make_shared<FastLogger>(serial, handler.getRegisterTypeMap(), "log.csv");
        handler.attachLogger(logger);
        CommandLine cli(serial, handler);
        cli.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
