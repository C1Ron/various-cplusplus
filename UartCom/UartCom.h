#ifndef UARTCOM_H
#define UARTCOM_H

#include "UartComIF.h"
#include <string>
#include <stdexcept>

class UartCom : public UartComIF
{
public:
    UartCom();
    virtual ~UartCom();

    int ConfigureInterfaceAttributes(int fd, int baudrate, int timeoutTenthOfSec = -1) override;
    int Open(const char* device, int* handle) override;
    int Close(int* handle);
    int Write(const uint8_t* buffer, size_t bufferLength, int handle) override;
    int Read(uint8_t* buffer, size_t nBytes, int fd) override;
    int Read(uint8_t* buffer, int fd) override;
    void ConfigureByteCount(int fd, int mcount) override;
    void ConfigureTimeout(int fd, int timeoutTenthOfSec) override;

private:
    // Disable copy constructor and assignment
    UartCom(const UartCom&) = delete;
    UartCom& operator=(const UartCom&) = delete;
};

#endif // UARTCOM_H
