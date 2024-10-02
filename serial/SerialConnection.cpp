// SerialConnection.cpp
#include "SerialConnection.h"
#include <iostream>

SerialConnection::SerialConnection(const std::string& port, unsigned int baud_rate)
    : serial(io, port) 
{
    serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    serial.set_option(boost::asio::serial_port_base::character_size(8));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
}

void SerialConnection::sendFrame(const std::vector<uint8_t>& frame) 
{
    boost::asio::write(serial, boost::asio::buffer(frame));
}
std::vector<uint8_t> SerialConnection::readFrame() 
{
    std::vector<uint8_t> header(2);
    boost::asio::read(serial, boost::asio::buffer(header));
    size_t payloadLength = header[1];
    std::vector<uint8_t> response(payloadLength + 1); // Add 1 for CRC
    boost::asio::read(serial, boost::asio::buffer(response));

    // Allocate totalResponse with the combined size of header and response
    std::vector<uint8_t> totalResponse(header.size() + response.size());
    std::copy(header.begin(), header.end(), totalResponse.begin());
    std::copy(response.begin(), response.end(), totalResponse.begin() + header.size());
    return totalResponse;
}
std::vector<uint8_t> SerialConnection::readFrame(size_t size) 
{
    std::vector<uint8_t> response(size);
    boost::asio::read(serial, boost::asio::buffer(response));
    return response;
}


SerialConnection::~SerialConnection() 
{
    serial.close();
}
