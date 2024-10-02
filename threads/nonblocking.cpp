#include <iostream>
#include <chrono>
#include <thread>

void gatherData() {
    // Simulate data gathering
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Data gathering complete." << std::endl;
}

int main() {
    // Create a thread to gather data
    std::cout << "Executing dataThread" << std::endl;
    std::thread dataThread(gatherData);

    // Main thread can do other stuff here
    for (int i = 0; i < 5; ++i) {
        std::cout << "Main thread doing other work." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Join the data thread before exiting
    dataThread.join();
    std::cout << "Main thread finished." << std::endl;

    return 0;
}
