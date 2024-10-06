//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include "boost/asio.hpp"

int main()
{
    boost::asio::io_context io;
    boost::asio::steady_timer t1(io, boost::asio::chrono::seconds(5));
    boost::asio::steady_timer t2(io, boost::asio::chrono::milliseconds(3000));
    t1.wait();
    std::cout << "t1 expired." << std::endl;
    t2.wait();
    std::cout << "t2 expired." << std::endl;
    return 0;
}