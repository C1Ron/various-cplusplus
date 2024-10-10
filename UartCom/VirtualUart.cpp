#include "VirtualUart.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>

std::string VirtualUart::fgLogFile = "virtual-uart-info.txt";

VirtualUart::VirtualUart() : fVirtualUartThread(nullptr), fUartRead(""), fUartWrite(""), socat_pid(0) 
{
    
}

VirtualUart::~VirtualUart()
{
    std::cout << "Calling VirtualUart destructor..." << std::endl;
    if (fVirtualUartThread != nullptr && fVirtualUartThread->joinable()) {
        std::cout << "Joining the virtual UART thread..." << std::endl;
        fVirtualUartThread->join();
        delete fVirtualUartThread;
    }

    // Kill the socat process if it is still running
    if (socat_pid > 0) {
        std::cout << "Terminating socat process (pid: " << socat_pid << ")..." << std::endl;
        kill(socat_pid, SIGTERM);  // Send termination signal to socat
        waitpid(socat_pid, nullptr, 0);  // Wait for the process to finish
    }

    std::cout << "Done." << std::endl;
}

void VirtualUart::Run()
{
    std::cout << "Starting thread..." << std::endl;
    // Use a lambda to capture `this` and invoke the member function
    fVirtualUartThread = new std::thread([this]() { this->StartThread(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void VirtualUart::StartThread()
{
    std::string cmd = "socat -lf " + fgLogFile + " -d -d pty,raw,echo=0 pty,raw,echo=0";
    std::cout << "Running command: " << cmd << std::endl;

    // Fork a new process to run socat
    pid_t pid = fork();
    if (pid == 0) {
        // In child process, replace current process with socat
        execlp("socat", "socat", "-lf", fgLogFile.c_str(), "-d", "-d", "pty,raw,echo=0", "pty,raw,echo=0", (char*)nullptr);
        std::cerr << "Error executing socat command." << std::endl;
        _exit(1);  // If execlp fails, exit child process
    } else if (pid > 0) {
        // In parent process, store the socat process ID in the instance
        socat_pid = pid;
        std::cout << "Started socat with pid: " << socat_pid << std::endl;
    } else {
        std::cerr << "Failed to fork socat process." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));  // Give time for the operations
}

void VirtualUart::Generate()
{
    ScanUartPairs();
}

void VirtualUart::ScanUartPairs()
{
    int max_retries = 5;
    int retries = 0;

    while (retries < max_retries) {
        std::ifstream infile(fgLogFile);
        if (!infile) {
            std::cerr << "Failed to open log file: " << fgLogFile << " (retrying...)" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Retry after 1 second
            retries++;
            continue;
        }

        std::string line;
        std::vector<std::string> lines;
        while (std::getline(infile, line)) {
            lines.push_back(line);
        }

        if (lines.size() >= 2) {
            fUartRead = Scan(lines[0]);
            fUartWrite = Scan(lines[1]);

            std::ofstream uart1("virtual-uart1.txt");
            std::ofstream uart2("virtual-uart2.txt");
            uart1 << fUartRead << std::endl;
            uart2 << fUartWrite << std::endl;

            std::cout << "fUartRead set to: " << fUartRead << std::endl;
            std::cout << "fUartWrite set to: " << fUartWrite << std::endl;
            return;  // Successfully parsed UARTs
        }

        std::cerr << "Expected at least 2 lines in log file, got " << lines.size() << " (retrying...)" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Retry after 1 second
        retries++;
    }

    throw std::runtime_error("Failed to read UART pairs after multiple retries.");
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
