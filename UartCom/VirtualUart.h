#ifndef VIRTUALUART_H
#define VIRTUALUART_H

#include <thread>
#include <string>
#include <vector>
#include <sys/types.h>

class VirtualUart
{
public:
    VirtualUart();
    virtual ~VirtualUart();

    void Run();
    void ScanUartPairs();
    void Generate();

    std::string GetUartRead() const { return fUartRead; }
    std::string GetUartWrite() const { return fUartWrite; }
    void SwapUarts();

private:
    VirtualUart(const VirtualUart&) = delete;
    VirtualUart& operator=(const VirtualUart&) = delete;

    void DeleteFileLocal();
    void StartThread();  // No longer static
    std::string Scan(const std::string& line);

    std::thread* fVirtualUartThread = nullptr;
    std::string fUartRead;
    std::string fUartWrite;
    static std::string fgLogFile;
    pid_t socat_pid;  // Process ID of the socat process
};

#endif // VIRTUALUART_H
