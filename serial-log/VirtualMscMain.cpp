#include "VirtualMsc.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <vector>
#include <set>
#include <algorithm>
#include <dirent.h>

const int MAX_RETRIES = 10;
const int RETRY_DELAY_MS = 1000;

volatile sig_atomic_t keep_running = 1;
pid_t socat_pid = -1;

void signal_handler(int signum) 
{
    keep_running = 0;
    if (socat_pid > 0) {
        kill(socat_pid, SIGTERM);
    }
    std::cout << "signum = " << signum << std::endl;
}

std::set<std::string> getPtyDevices() 
{
    std::set<std::string> devices;
    DIR* dir = opendir("/dev/pts");
    if (dir == nullptr) {
        std::cerr << "Failed to open /dev/pts directory" << std::endl;
        return devices;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_CHR && std::string(entry->d_name) != "ptmx") {
            std::string path = "/dev/pts/" + std::string(entry->d_name);
            devices.insert(path);
        }
    }

    closedir(dir);
    return devices;
}

void printHex(const std::vector<uint8_t>& data, const std::string& label) 
{
    std::cout << label << ":\t";
    for (const auto& byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void printDevices(const std::set<std::string>& devices, const std::string& label) 
{
    std::cout << label << " devices:" << std::endl;
    for (const auto& device : devices) {
        std::cout << "  " << device << std::endl;
    }
}

std::vector<std::string> findNewPtyDevices(const std::set<std::string>& before, const std::set<std::string>& after) 
{
    std::vector<std::string> diff;
    std::set_difference(after.begin(), after.end(), before.begin(), before.end(), 
                        std::back_inserter(diff));
    return diff;
}

int openSerialPort(const char* portName) 
{
    int fd = -1;
    for (int i = 0; i < MAX_RETRIES; ++i) {
        fd = open(portName, O_RDWR | O_NOCTTY);
        if (fd >= 0) {
            break;
        }
        std::cerr << "Failed to open " << portName << ". Retrying in " << RETRY_DELAY_MS << "ms..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }

    if (fd < 0) {
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Failed to get serial port attributes" << std::endl;
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Failed to set serial port attributes" << std::endl;
        close(fd);
        return -1;
    }

    return fd;
}

int main() 
{
    signal(SIGINT, signal_handler);

    // Get the list of PTY devices before starting socat
    std::set<std::string> before_devices = getPtyDevices();
    // Start socat in the background
    socat_pid = fork();
    if (socat_pid == 0) {
        // Child process
        execlp("socat", "socat", "pty,raw,echo=0", "pty,raw,echo=0", NULL);
        std::cerr << "Failed to execute socat" << std::endl;
        exit(1);  // If execlp fails
    } else if (socat_pid < 0) {
        std::cerr << "Failed to fork for socat" << std::endl;
        return 1;
    }

    // Wait for socat to create the PTY devices

    std::vector<std::string> new_devices;
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::set<std::string> after_devices = getPtyDevices();
        new_devices = findNewPtyDevices(before_devices, after_devices);
        if (new_devices.size() == 2) {
            std::cout << "Found 2 new PTY devices after " << i+1 << " seconds" << std::endl;
            break;
        }
        std::cout << "Attempt " << i+1 << ": Found " << new_devices.size() << " new devices" << std::endl;
        printDevices(std::set<std::string>(new_devices.begin(), new_devices.end()), "New (incomplete)");
    }

    if (new_devices.size() != 2) {
        std::cerr << "Failed to find exactly two new PTY devices" << std::endl;
        printDevices(getPtyDevices(), "Current");
        kill(socat_pid, SIGTERM);
        return 1;
    }

    std::string port1 = new_devices[0];
    std::string port2 = new_devices[1];

    std::cout << "Virtual serial ports created: " << port1 << " and " << port2 << std::endl;
    std::cout << "Use " << port2 << " to connect to the virtual MSC" << std::endl;
    std::cout << "Waiting for connection..." << std::endl;

    int fd = openSerialPort(port1.c_str());
    if (fd < 0) {
        std::cerr << "Failed to open virtual serial port after multiple attempts" << std::endl;
        kill(socat_pid, SIGTERM);
        return 1;
    }

    VirtualMsc msc;

    std::cout << "Virtual MSC is running. Press Ctrl+C to exit." << std::endl;

    while (keep_running) {
        std::vector<uint8_t> buffer(256);
        ssize_t bytesRead = read(fd, buffer.data(), buffer.size());

        if (bytesRead > 0) {
            buffer.resize(bytesRead);
            printHex(buffer, "Received");

            std::vector<uint8_t> response = msc.processFrame(buffer);
            printHex(response, "Sending");

            ssize_t bytesWritten = write(fd, response.data(), response.size());
            if (bytesWritten != static_cast<ssize_t>(response.size())) {
                std::cerr << "Failed to write full response. Wrote " << bytesWritten << " of " << response.size() << " bytes." << std::endl;
            }

            // Flush the output
            tcdrain(fd);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "Shutting down..." << std::endl;
    close(fd);
    kill(socat_pid, SIGTERM);
    return 0;
}