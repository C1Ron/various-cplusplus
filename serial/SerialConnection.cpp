#include "SerialConnection.h"
#include <iostream>

SerialConnection::SerialConnection(const std::string& port, unsigned int baud_rate)
    : serial(io, port) {
    configurePort(baud_rate);
}

SerialConnection::~SerialConnection() {
    try {
        if (serial.is_open()) {
            serial.close();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error closing serial port: " << e.what() << std::endl;
    }
}

void SerialConnection::configurePort(unsigned int baud_rate) {
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialConnection::setTimeout(const std::chrono::milliseconds& timeout) {
    readTimeout = timeout;
}

void SerialConnection::sendFrame(const std::vector<uint8_t>& frame) {
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        boost::asio::write(serial, boost::asio::buffer(frame));
    } catch (const std::exception& e) {
        throw ReadError("Error sending frame: " + std::string(e.what()));
    }
}

std::vector<uint8_t> SerialConnection::readFrame() {
    std::lock_guard<std::mutex> lock(serialMutex);
    try {
        // Read header first (2 bytes)
        std::vector<uint8_t> header = readWithTimeout(2);
        if (header.size() != 2) {
            throw ReadError("Failed to read header");
        }

        size_t payloadLength = header[1];
        size_t totalLength = payloadLength + 1; // +1 for CRC

        // Read rest of frame
        std::vector<uint8_t> payload = readWithTimeout(totalLength);
        if (payload.size() != totalLength) {
            throw ReadError("Failed to read complete frame");
        }

        // Combine header and payload
        std::vector<uint8_t> complete;
        complete.reserve(header.size() + payload.size());
        complete.insert(complete.end(), header.begin(), header.end());
        complete.insert(complete.end(), payload.begin(), payload.end());

        return complete;
    }
    catch (const std::exception& e) {
        // Clear any partial data that might be in the buffer
        boost::system::error_code ec;
        serial.cancel(ec); // Cancel any pending operations
        throw ReadError(std::string("Read frame error: ") + e.what());
    }
}

std::vector<uint8_t> SerialConnection::readFrame(size_t size) {
    std::lock_guard<std::mutex> lock(serialMutex);
    return readWithTimeout(size);
}

std::vector<uint8_t> SerialConnection::readWithTimeout(size_t size) {
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
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100ms timeout

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