#include "SerialConnection.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

SerialConnection::SerialConnection(const std::string& port, unsigned int baud_rate)
    : io(), serial(io), readTimeout(1000) // Default 1 second timeout
{
    serial.open(port);
    configurePort(baud_rate);
}

SerialConnection::~SerialConnection() 
{
    try {
        if (serial.is_open()) {
            serial.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error closing serial port: " << e.what() << std::endl;
    }
    std::cout << "SerialConnection destructor called" << std::endl;
}

void SerialConnection::configurePort(unsigned int baud_rate)
{
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialConnection::sendFrame(const std::vector<uint8_t>& frame) 
{
    std::lock_guard<std::mutex> lock(serialMutex);  // Lock mutex for thread safety
    try {
        boost::asio::write(serial, boost::asio::buffer(frame));
    } catch (const std::exception& e) {
        throw std::runtime_error("Error sending frame: " + std::string(e.what()));
    }
}

std::vector<uint8_t> SerialConnection::readFrame() 
{
    std::lock_guard<std::mutex> lock(serialMutex);  // Lock mutex for thread safety
    try {
        std::vector<uint8_t> header = readWithTimeout(2);
        size_t payloadLength = header[1];
        std::vector<uint8_t> response = readWithTimeout(payloadLength + 1); // Add 1 for CRC

        std::vector<uint8_t> totalResponse(header.size() + response.size());
        std::copy(header.begin(), header.end(), totalResponse.begin());
        std::copy(response.begin(), response.end(), totalResponse.begin() + header.size());
        return totalResponse;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading frame: " + std::string(e.what()));
    }
}

std::vector<uint8_t> SerialConnection::readFrame(size_t size) 
{
    std::lock_guard<std::mutex> lock(serialMutex);  // Lock mutex for thread safety
    return readWithTimeout(size);
}

void SerialConnection::setTimeout(const std::chrono::milliseconds& timeout)
{
    readTimeout = timeout;
}

std::vector<uint8_t> SerialConnection::readWithTimeout(size_t size)
{
    std::vector<uint8_t> response(size);
    boost::system::error_code ec;
    boost::asio::async_read(serial, boost::asio::buffer(response),
        [&ec](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
            ec = error;
        });

    io.run_for(readTimeout);

    if (ec) {
        if (ec == boost::asio::error::operation_aborted) {
            throw std::runtime_error("Read operation timed out");
        }
        throw std::runtime_error("Read error: " + ec.message());
    }

    io.reset();
    return response;
}
