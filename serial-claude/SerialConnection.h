#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>  // Include for thread synchronization

class SerialConnection {
public:
    SerialConnection(const std::string& port, unsigned int baud_rate);
    ~SerialConnection();

    void sendFrame(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> readFrame();
    std::vector<uint8_t> readFrame(size_t size);
    
    void setTimeout(const std::chrono::milliseconds& timeout);

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    std::chrono::milliseconds readTimeout;
    std::mutex serialMutex;  // Mutex to protect access to the serial port

    void configurePort(unsigned int baud_rate);
    std::vector<uint8_t> readWithTimeout(size_t size);
};

#endif // SERIAL_CONNECTION_H
