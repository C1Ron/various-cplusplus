#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <utility>  // known issue with boost::asio and C++17
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>

class SerialConnection {
public:
    class ReadError : public std::runtime_error {
    public:
        explicit ReadError(const std::string& msg) : std::runtime_error(msg) {}
    };

    SerialConnection(const std::string& port, unsigned int baud_rate);
    ~SerialConnection();

    void sendFrame(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> readFrame();
    std::vector<uint8_t> readFrame(size_t size);
    
    void setTimeout(const std::chrono::milliseconds& timeout);

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    std::chrono::milliseconds readTimeout{1000}; // Default 1 second timeout
    std::mutex serialMutex;
    
    std::vector<uint8_t> readWithTimeout(size_t size);
    void configurePort(unsigned int baud_rate);
};

#endif // SERIAL_CONNECTION_H