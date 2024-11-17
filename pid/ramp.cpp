#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <iomanip>
#include <string>

int main() {
    double currentSpeed, finalSpeed, duration;
    const double MAX_DURATION = 10000; // 10 seconds in milliseconds
    const int BAR_WIDTH = 50;  // Width of visualization bar
    
    // Get user input
    std::cout << "Enter current speed: ";
    std::cin >> currentSpeed;
    
    std::cout << "Enter final speed: ";
    std::cin >> finalSpeed;
    
    do {
        std::cout << "Enter duration (milliseconds, max 10000): ";
        std::cin >> duration;
        if (duration > MAX_DURATION) {
            std::cout << "Duration too long. Maximum is 10000ms (10 seconds)\n";
        }
    } while (duration > MAX_DURATION);
    
    // Calculate marker positions
    int startMarker = 0;
    int endMarker = static_cast<int>((duration / MAX_DURATION) * BAR_WIDTH);
    if (endMarker >= BAR_WIDTH) endMarker = BAR_WIDTH - 1;
    
    // Calculate step size for smooth ramping
    const int updateInterval = 20;  // milliseconds
    int steps = duration / updateInterval;
    double speedDifference = finalSpeed - currentSpeed;
    
    // Store start time
    auto startTime = std::chrono::steady_clock::now();
    double elapsedTimeMs = 0;
    double speed = currentSpeed;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nRamping speed from " << currentSpeed << " to " << finalSpeed 
              << " over " << duration << "ms\n\n";
    
    // Show the scale
    std::cout << "Time scale (ms):  0";
    std::cout << std::string((BAR_WIDTH/2) - 5, ' ') << (MAX_DURATION/2);
    std::cout << std::string((BAR_WIDTH/2) - 5, ' ') << MAX_DURATION << "\n";
    
    // Show the markers
    std::cout << "Speed markers:   ";
    for (int i = 0; i < BAR_WIDTH; i++) {
        if (i == startMarker) {
            std::cout << "S";  // Start marker
        }
        else if (i == endMarker) {
            std::cout << "E";  // End marker
        }
        else {
            std::cout << "-";
        }
    }
    std::cout << "\n              S=" << currentSpeed << " E=" << finalSpeed << "\n\n";
    
    // Ramp the speed
    while (elapsedTimeMs < duration) {
        // Calculate elapsed time
        auto currentTime = std::chrono::steady_clock::now();
        elapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>
                        (currentTime - startTime).count();
        
        // Update speed using linear interpolation
        speed = currentSpeed + (speedDifference * (elapsedTimeMs / duration));
        
        // Calculate current position
        int currentPos = static_cast<int>((elapsedTimeMs / MAX_DURATION) * BAR_WIDTH);
        if (currentPos >= BAR_WIDTH) currentPos = BAR_WIDTH - 1;
        
        // Print speed with progress bar
        std::cout << "Speed: " << std::setw(8) << speed << " [";
        for (int i = 0; i < BAR_WIDTH; i++) {
            if (i == currentPos) {
                std::cout << "*";  // Current position
            }
            else if (i == startMarker) {
                std::cout << "S";  // Start marker
            }
            else if (i == endMarker) {
                std::cout << "E";  // End marker
            }
            else {
                std::cout << "-";
            }
        }
        std::cout << "]\r";
        std::cout.flush();
        
        // Sleep for update interval
        std::this_thread::sleep_for(std::chrono::milliseconds(updateInterval));
    }
    
    // Final update
    std::cout << "Speed: " << std::setw(8) << finalSpeed << " [";
    for (int i = 0; i < BAR_WIDTH; i++) {
        if (i == endMarker) {
            std::cout << "*";  // Final position
        }
        else if (i == startMarker) {
            std::cout << "S";  // Start marker
        }
        else {
            std::cout << "-";
        }
    }
    std::cout << "]\n\nRamping complete!" << std::endl;
    
    return 0;
}