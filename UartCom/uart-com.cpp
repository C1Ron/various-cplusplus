#include "UartApplication.h"
#include <iostream>

int main(int argc, const char** argv)
{
    try {
        UartApplication app;
        app.ScanArguments(argc, argv);
        app.Open();
        app.Execute();
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return -1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}