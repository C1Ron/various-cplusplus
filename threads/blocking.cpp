#include <iostream>
#include <chrono>
#include <thread>
int main() {
    std::cout << "Executing a lambda-function thread for five seconds!" << std::endl;
    std::thread my_thread([]() {
        // Simulate some work in the thread
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Thread work done." << std::endl;
    });

    my_thread.join(); // Main thread will block here until my_thread finishes
    std::cout << "Main thread continues after my_thread has finished." << std::endl;

    return 0;
}
