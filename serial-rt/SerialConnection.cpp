#include "SerialConnection.h"
#include <iostream>

SerialConnection::SerialConnection(const std::string& port, unsigned int baud_rate)
    : serial(io, port) 
{
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
}

void SerialConnection::configurePort(unsigned int baud_rate) 
{
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialConnection::setTimeout(const std::chrono::milliseconds& timeout) 
{
    readTimeout = timeout;
}

void SerialConnection::sendFrame(const std::vector<uint8_t>& frame) 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        boost::asio::write(serial, boost::asio::buffer(frame));
    } catch (const std::exception& e) {
        throw ReadError("Error sending frame: " + std::string(e.what()));
    }
}

std::vector<uint8_t> SerialConnection::readFrame() 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        // Read first two bytes
        std::vector<uint8_t> header = readWithTimeout(2);
        if (header.size() != 2) {
            throw ReadError("Failed to read header");
        }

        // Debugging
        std::cout << "SerialConnection::readFrame(): \n";
        for (const auto& byte : header) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;

        // Total size is in second byte
        size_t totalSize = header[1];
        if (totalSize < 2) {
            throw ReadError("Invalid total size");
        }

        // Read remaining bytes (totalSize - 2 since we already read 2)
        size_t remainingBytes = totalSize - 2;
        std::vector<uint8_t> remaining = readWithTimeout(remainingBytes);
        if (remaining.size() != remainingBytes) {
            throw ReadError("Failed to read complete frame");
        }

        // Debugging
        std::cout << "SerialConnection::readFrame(): remaining\n";
        for (const auto& byte : remaining) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;

        // Combine header and remaining bytes
        std::vector<uint8_t> complete;
        complete.reserve(totalSize);
        complete.insert(complete.end(), header.begin(), header.end());
        complete.insert(complete.end(), remaining.begin(), remaining.end());

        return complete;
    }
    catch (const std::exception& e) {
        boost::system::error_code ec;
        serial.cancel(ec);
        throw ReadError(std::string("Read frame error: ") + e.what());
    }
}

std::vector<uint8_t> SerialConnection::readFrame(size_t size) 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    return readWithTimeout(size);
}

std::vector<uint8_t> SerialConnection::readWithTimeout(size_t size) 
{
    std::vector<uint8_t> buffer(size);
    
    // Simple synchronous read with timeout
    boost::asio::async_read(serial,
        boost::asio::buffer(buffer),
        [](const boost::system::error_code&, size_t){});
        
    if (!serial.native_handle()) {
        throw ReadError("Invalid serial handle");
    }

    // Use select to implement timeout
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(serial.native_handle(), &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int result = select(serial.native_handle() + 1, &read_fds, nullptr, nullptr, &timeout);
    if (result < 0) {
        throw ReadError("Select error");
    }
    if (result == 0) {
        throw ReadError("Timeout");
    }

    // Perform the actual read
    size_t bytesRead = boost::asio::read(serial, boost::asio::buffer(buffer));
    if (bytesRead != size) {
        throw ReadError("Incomplete read");
    }

    return buffer;
}