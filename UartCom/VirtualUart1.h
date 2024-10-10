#ifndef VIRTUALUART1_H
#define VIRTUALUART1_H

#include <thread>
#include <string>
#include <vector>

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
    static void StartThread();
    std::string Scan(const std::string& line);

    std::thread* fVirtualUartThread = nullptr;
    std::string fUartRead;
    std::string fUartWrite;
    static std::string fgLogFile;
};

#endif // VIRTUALUART_H
