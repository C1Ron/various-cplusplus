#include "RtInterface.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <serial-port> <msc-id>\n";
        return 1;
    }

    try {
        uint8_t mscId = static_cast<uint8_t>(std::stoi(argv[2]));
        RtInterface interface(argv[1], 115200, mscId);
        interface.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}