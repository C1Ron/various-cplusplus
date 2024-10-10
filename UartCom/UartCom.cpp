#include "UartCom.h"
#include <iostream>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

UartCom::UartCom() {}

UartCom::~UartCom() {}

int UartCom::ConfigureInterfaceAttributes(int fd, int baudrate, int timeoutTenthOfSec)
{
    struct termios attributes;

    if (tcgetattr(fd, &attributes) < 0) {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
        return -1;
    }

    attributes.c_cflag = baudrate | CS8 | CLOCAL | CREAD;
    attributes.c_iflag = IGNPAR;
    attributes.c_oflag = 0;
    attributes.c_lflag = 0;

    if (timeoutTenthOfSec > 0) {
        attributes.c_cc[VTIME] = timeoutTenthOfSec;
        attributes.c_cc[VMIN] = 0;
    }

    if (tcsetattr(fd, TCSANOW, &attributes) != 0) {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int UartCom::Open(const char* device, int* handle)
{
    *handle = open(device, O_RDWR | O_NOCTTY | O_SYNC);

    if (*handle < 0) {
        std::cerr << "Error opening " << device << ": " << strerror(errno) << std::endl;
        return -1;
    }

    return 0;
}

int UartCom::Close(int* handle)
{
    int ret = close(*handle);
    if (ret == -1) {
        std::cerr << "Error closing device, fd = " << *handle << std::endl;
    }
    return ret;
}

int UartCom::Write(const uint8_t* buffer, size_t bufferLength, int handle)
{
    int wlen = write(handle, buffer, bufferLength);
    if (wlen == -1) {
        std::cerr << "Error from write: " << strerror(errno) << std::endl;
        return -1;
    } else if (wlen < static_cast<int>(bufferLength)) {
        std::cerr << "Warning: Written bytes (" << wlen << ") less than expected (" << bufferLength << ")" << std::endl;
        return wlen;
    }
    return wlen;
}


int UartCom::Read(uint8_t* buffer, const size_t nBytes, const int fd)
{
    fd_set set;
    struct timeval timeout;

    // Set timeout to 5 seconds
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Wait for the file descriptor to be ready for reading
    int rv = select(fd + 1, &set, NULL, NULL, &timeout);
    if (rv == -1) {
        std::cerr << "Error from select: " << strerror(errno) << std::endl;
        return -1;
    } else if (rv == 0) {
        std::cerr << "Timeout: No data available for reading." << std::endl;
        return 0;
    }

    // Try to read the specified number of bytes from the file descriptor
    int rdlen = read(fd, buffer, nBytes);
    if (rdlen == -1) {
        std::cerr << "Error from read: " << strerror(errno) << std::endl;
        return -1;
    } else if (rdlen == 0) {
        std::cerr << "EOF reached: No data read." << std::endl;
    }

    // Null-terminate the string for safety
    if (rdlen > 0 && rdlen < static_cast<int>(nBytes)) {
        buffer[rdlen] = '\0';
    }

    return rdlen;
}




int UartCom::Read(uint8_t* buffer, int fd)
{
    return Read(buffer, 4096, fd); // default buffer size
}

void UartCom::ConfigureTimeout(int fd, int timeoutTenthOfSec)
{
    struct termios attributes;
    if (tcgetattr(fd, &attributes) < 0) {
        std::cerr << "Error tcgetattr: " << strerror(errno) << std::endl;
        return;
    }

    attributes.c_cc[VTIME] = timeoutTenthOfSec;

    if (tcsetattr(fd, TCSANOW, &attributes) != 0) {
        std::cerr << "Error tcsetattr: " << strerror(errno) << std::endl;
    }
}

void UartCom::ConfigureByteCount(int fd, int mcount)
{
    struct termios attributes;
    if (tcgetattr(fd, &attributes) < 0) {
        std::cerr << "Error tcgetattr: " << strerror(errno) << std::endl;
        return;
    }

    attributes.c_cc[VMIN] = mcount;

    if (tcsetattr(fd, TCSANOW, &attributes) < 0) {
        std::cerr << "Error tcsetattr: " << strerror(errno) << std::endl;
    }
}
