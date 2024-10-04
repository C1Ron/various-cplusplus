#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    std::string cmd = argv[1];
    if (cmd == "read") {
        std::cout << "Read command received." << std::endl;
    } else if (cmd == "write") {
        if (argc > 2){
            std::cout << "Write command with argument \"" << argv[2] << "\" received." << std::endl;
        } else {
            std::cout << "Write command with no arguments received." << std::endl;
        }
        
    } else {
        std::cout << "Neither read nor write was found.\n\tcmd = " << cmd << std::endl; 
    }

    return 0;
}