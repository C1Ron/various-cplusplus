#include "VirtualUart1.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>

std::string VirtualUart::fgLogFile = "virtual-uart-info.txt";

VirtualUart::VirtualUart() : fVirtualUartThread(nullptr), fUartRead(""), fUartWrite("") {}

VirtualUart::~VirtualUart()
{
    if (fVirtualUartThread != nullptr) {
        fVirtualUartThread->join();
        delete fVirtualUartThread;
    }
}

void VirtualUart::Run()
{
    std::cout << "Starting thread..." << std::endl;
    fVirtualUartThread = new std::thread(&StartThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void VirtualUart::DeleteFileLocal()
{
    std::ifstream infile(fgLogFile);
    if (infile.good()) {
        std::remove(fgLogFile.c_str());
    }
}

void VirtualUart::StartThread()
{
    std::string cmd = "socat -lf " + fgLogFile + " -d -d pty,raw,echo=0 pty,raw,echo=0";
    std::cout << "Running command: " << cmd << std::endl;

    // Execute the command (assuming a UNIX-like environment)
    int ret = system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "Error executing socat command." << std::endl;
    }
}

void VirtualUart::Generate()
{
    ScanUartPairs();
}

void VirtualUart::ScanUartPairs()
{
    std::ifstream infile(fgLogFile);
    if (!infile) {
        throw std::runtime_error("Failed to open log file: " + fgLogFile);
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }

    if (lines.size() < 2) {
        throw std::runtime_error("Expected at least 2 lines in " + fgLogFile + ", got " + std::to_string(lines.size()));
    }

    fUartRead = Scan(lines[0]);
    fUartWrite = Scan(lines[1]);

    std::ofstream uart1("virtual-uart1.txt");
    std::ofstream uart2("virtual-uart2.txt");
    uart1 << fUartRead << std::endl;
    uart2 << fUartWrite << std::endl;

    std::cout << "fUartRead set to: " << fUartRead << std::endl;
    std::cout << "fUartWrite set to: " << fUartWrite << std::endl;
}

std::string VirtualUart::Scan(const std::string& line)
{
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() != 7) {
        throw std::runtime_error("Unexpected number of tokens in line: " + line);
    }

    return tokens[6];
}

void VirtualUart::SwapUarts()
{
    std::swap(fUartRead, fUartWrite);
}
