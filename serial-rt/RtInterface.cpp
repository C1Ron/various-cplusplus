#include "RtInterface.h"
#include <readline/readline.h>
#include <readline/history.h>

RtInterface::RtInterface(const std::string& port, unsigned int baudRate, uint8_t mscId)
    : mscId(mscId)
{
    serial = std::make_unique<SerialConnectionRt>(port, baudRate);
    logger = std::make_unique<LoggerRt>(*serial, mscId, createLogConfig());
    handler = std::make_unique<CommandHandlerRt>(*serial, mscId, *logger);
    setupSignalHandler();
}

RtInterface::~RtInterface()
{
    cleanup();
}

void RtInterface::run()
{
    while (!SignalHandler::shouldExit()) {
        char* line = readline("rt> ");
        if (line == nullptr) {
            std::cout << "CTRL+D detected. Exiting..." << std::endl;
            break;
        }

        std::string userInput(line);
        if (!userInput.empty()) {
            add_history(line);
            processUserInput(userInput);
        }
        free(line);
    }
}

void RtInterface::setupSignalHandler()
{
    SignalHandler::reset();
    SignalHandler::setup([this] { cleanup(); });
}

void RtInterface::cleanup()
{
    if (logger) {
        std::cout << "RtInterface::cleanup() - Stopping logger..." << std::endl;
        logger->stop();
    }
}

void RtInterface::processUserInput(const std::string& userInput)
{
    if (userInput == "help") {
        printHelp();
    } else if (userInput == "help-reg") {
        printRegisters(*handler);
    } else if (userInput == "help-exec") {
        printExecutes(*handler);
    } else if (userInput == "help-foc-reg") {
        printFocRegisters(*handler);
    } else if (userInput == "help-foc-exec") {
        printFocExecutes(*handler);
    } else if (userInput == "exit") {
        std::cout << "Exiting..." << std::endl;
        SignalHandler::shouldExit(true);
    } else {
        processCommand(userInput);
    }
}

void RtInterface::printHelp()
{
    std::cout 
    << "RT commands:=========================================================================\n"
    << "\tread       <reg>                - Read register value\n"
    << "\twrite      <reg>    <val>       - Write register value\n"
    << "\texec       <cmd>                - Execute command\n"
    << "\texec ramp  <speed>  <time>      - Execute speed ramp\n"
    << "FOC commands:========================================================================\n"
    << "\tfoc-read   <reg>                - Read FOC register value\n"
    << "\tfoc-write  <reg>    <val>       - Write FOC register value\n"
    << "\tfoc-exec   <cmd>                - Execute FOC command\n"
    << "Logging commands:====================================================================\n"
    << "\tlog-start                       - Start logging\n"
    << "\tlog-stop                        - Stop logging\n"
    << "\tlog-add-rt     <reg>            - Add register to logging\n"
    << "\tlog-remove-rt  <reg>            - Remove register from logging\n"
    << "\tlog-add-foc    <reg>            - Add FOC register to logging\n"
    << "\tlog-remove-foc <reg>            - Remove FOC register from logging\n"
    << "\tlog-status                      - Show logging status\n"
    << "\tlog-config   <fname> <interval> - Update logging configuration\n"
    << "Other commands:======================================================================\n"
    << "\thelp                            - Show this help\n"
    << "\thelp-reg                        - Show all available registers and associated types\n"
    << "\thelp-exec                       - Show all available execute commands\n"
    << "\thelp-foc-reg                    - Show all available FOC registers and associated types\n"
    << "\thelp-foc-exec                   - Show all available FOC execute commands\n"
    << "\texit                            - Exit program\n"
    << "\tCTRL+D                          - Exit program\n";
}

void RtInterface::processCommand(const std::string& command)
{
    auto result = handler->processCommand(command);
    if (result.success) {
        std::cout << result.message << std::endl;
    } else {
        std::cerr << "Error: " << result.message << std::endl;
    }
}

LoggerRt::LogConfig RtInterface::createLogConfig()
{
    return {
        .filename = "rt_log.csv",
        .sampleInterval = std::chrono::milliseconds(100),
        .bufferSize = 1024,
        .useTimestamp = true
    };
}

void RtInterface::printRegisters(const CommandHandlerRt& handler)
{
    std::cout << handler.printAllRegisters() << std::endl;
}

void RtInterface::printExecutes(const CommandHandlerRt& handler)
{
    std::cout << handler.printAllExecutes() << std::endl;
}

void RtInterface::printFocRegisters(const CommandHandlerRt& handler)
{
    std::cout << "RtInterface::printFocRegisters() \n";
    std::cout << handler.printAllFocRegisters() << std::endl;
}

void RtInterface::printFocExecutes(const CommandHandlerRt& handler)
{
    std::cout << handler.printAllFocExecutes() << std::endl;
}