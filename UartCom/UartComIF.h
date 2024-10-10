#ifndef UARTCOMIF_H
#define UARTCOMIF_H

#include <cstdint>
#include <cstddef>

class UartComIF
{
public:
    UartComIF() = default;
    virtual ~UartComIF() = default;

    virtual int ConfigureInterfaceAttributes(int fd, int baudrate, int timeoutTenthOfSec = -1) = 0;
    virtual int Open(const char* device, int* handle) = 0;
    virtual int Write(const uint8_t* buffer, size_t bufferLength, int handle) = 0;
    virtual int Read(uint8_t* buffer, size_t nBytes, int fd) = 0;
    virtual int Read(uint8_t* buffer, int fd) = 0;
    virtual void ConfigureByteCount(int fd, int mcount) = 0;
    virtual void ConfigureTimeout(int fd, int timeoutTenthOfSec) = 0;
};

#endif // UARTCOMIF_H
