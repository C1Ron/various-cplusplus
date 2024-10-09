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
        CommandHandler handler(serial);

        FastLogger logger(serial, handler, "log.csv");
        logger.addRegister(ST_MPC::RegisterId::SpeedRef);
        logger.addRegister(ST_MPC::RegisterId::TorqueKp);

        if (!logger.removeRegister(ST_MPC::RegisterId::TorqueKp)) {
            std::cerr << "Could not remove register" << std::endl;
        }

        logger.startLogging();

        CommandLine cli(serial);
        cli.run();

        logger.stopLogging();
        std::cout << "Logging stopped." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
