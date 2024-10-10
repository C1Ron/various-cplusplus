#include "UartApplication.h"
#include <iostream>

int main(int argc, const char** argv)
{
    try {
        UartApplication app;
        app.ScanArguments(argc, argv);
        app.Execute();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
