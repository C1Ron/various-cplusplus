#include "SerialConnectionRt.h"
#include <iostream>

SerialConnectionRt::SerialConnectionRt(const std::string& port, unsigned int baud_rate)
    : serial(io, port) 
{
    configurePort(baud_rate);
}

SerialConnectionRt::~SerialConnectionRt() 
{
    try {
        if (serial.is_open()) {
            serial.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error closing serial port: " << e.what() << std::endl;
    }
}

void SerialConnectionRt::configurePort(unsigned int baud_rate) 
{
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialConnectionRt::setTimeout(const std::chrono::milliseconds& timeout) 
{
    readTimeout = timeout;
}

void SerialConnectionRt::sendFrame(const std::vector<uint8_t>& frame) 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        // Clear any pending data first
        clearInputBuffer();
        
        // Then send the frame
        boost::asio::write(serial, boost::asio::buffer(frame));
    } catch (const std::exception& e) {
        throw ReadError("Error sending frame: " + std::string(e.what()));
    }
}

std::vector<uint8_t> SerialConnectionRt::readFrame() 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        // Read header (16 bytes)
        std::vector<uint8_t> frame = readWithTimeout(16);
        if (frame.size() != 16) {
            throw ReadError("Failed to read header");
        }

        // Get total size from header
        size_t totalSize = frame[1];
        if (totalSize < 16) {
            throw ReadError("Invalid total size");
        }

        // Read remaining bytes if any
        if (totalSize > 16) {
            size_t remainingBytes = totalSize - 16;
            std::vector<uint8_t> remaining = readWithTimeout(remainingBytes);
            if (remaining.size() != remainingBytes) {
                throw ReadError("Failed to read complete frame");
            }
            frame.insert(frame.end(), remaining.begin(), remaining.end());
        }

        return frame;
    }
    catch (const std::exception& e) {
        boost::system::error_code ec;
        serial.cancel(ec);
        throw ReadError(std::string("Read frame error: ") + e.what());
    }
}

std::vector<uint8_t> SerialConnectionRt::readFrame(size_t size) 
{
    std::lock_guard<std::mutex> lock(serialMutex);
    return readWithTimeout(size);
}

std::vector<uint8_t> SerialConnectionRt::readWithTimeout(size_t size) 
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

void SerialConnectionRt::clearInputBuffer()
{
    // Read any pending data with a very short timeout
    struct timeval shortTimeout;
    shortTimeout.tv_sec = 0;
    shortTimeout.tv_usec = 1000;  // 1ms
    
    std::vector<uint8_t> buffer(256);
    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(serial.native_handle(), &read_fds);
        
        int result = select(serial.native_handle() + 1, &read_fds, nullptr, nullptr, &shortTimeout);
        if (result <= 0) {
            break;  // No more data or error
        }
        
        // Read and discard data
        boost::system::error_code ec;
        serial.read_some(boost::asio::buffer(buffer), ec);
        if (ec) break;
    }
}