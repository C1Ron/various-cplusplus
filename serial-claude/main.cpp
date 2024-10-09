#include "CommandLine.h"
#include "SerialConnection.h"
#include "SignalHandler.h"
#include "FastLogger.h"
#include "CommandHandler.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return 1;
    }

    const std::string port = argv[1];

    try {
        SerialConnection serial(port, 115200);

        // Create CommandHandler
        CommandHandler handler(serial);

        // Initialize FastLogger with CommandHandler
        FastLogger logger(serial, handler, "log.csv");

        // Add registers to log
        logger.addRegister(ST_MPC::RegisterId::SpeedRef);
        logger.addRegister(ST_MPC::RegisterId::TorqueKp);

        // Start the logger
        logger.startLogging();
        std::cout << "Logging started..." << std::endl;

        // Start the command line interface
        CommandLine cli(serial);
        cli.run();

        // Stop the logger after CommandLine exits
        logger.stopLogging();
        std::cout << "Logging stopped." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
