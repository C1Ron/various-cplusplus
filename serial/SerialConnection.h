// SerialConnection.h
#ifndef SERIAL_CONNECTION_H
#define SERIAL_CONNECTION_H

#include <boost/asio.hpp>
#include <string>
#include <vector>

class SerialConnection {
public:
    SerialConnection(const std::string& port, unsigned int baud_rate);
    void sendFrame(const std::vector<uint8_t>& frame);
    std::vector<uint8_t> readFrame();
    std::vector<uint8_t> readFrame(size_t size);
    ~SerialConnection();

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
};

#endif // SERIAL_CONNECTION_H
