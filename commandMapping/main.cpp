#include <iostream>
#include <unordered_map>
#include <functional>

// Custom hash function for std::pair
struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // Combine the two hashes
    }
};

// Command type alias
using Command = std::function<void(int)>;

// Define the command map with the custom hash function
std::unordered_map<std::pair<int, int>, Command, PairHash> commandMap;

// Example command functions
void readTarget1(int reg) {
    std::cout << "Reading from target 1, register " << reg << std::endl;
}

void readTarget2(int reg) {
    std::cout << "Reading from target 2, register " << reg << std::endl;
}

void writeTarget1(int reg, int value) {
    std::cout << "Writing value " << value << " to target 1, register " << reg << std::endl;
}

void writeTarget2(int reg, int value) {
    std::cout << "Writing value " << value << " to target 2, register " << reg << std::endl;
}

// Setup commands
void setupCommands() {
    commandMap[{3, 1}] = [](int) { readTarget1(1); };
    commandMap[{3, 2}] = [](int) { readTarget1(2); };
    commandMap[{3, 3}] = [](int) { readTarget1(3); };
    commandMap[{3, 4}] = [](int) { readTarget1(4); };
    commandMap[{3, 5}] = [](int) { readTarget2(1); };
    commandMap[{3, 6}] = [](int) { readTarget2(2); };
    commandMap[{3, 7}] = [](int) { readTarget2(3); };
    commandMap[{3, 8}] = [](int) { readTarget2(4); };

    commandMap[{5, 1}] = [](int value) { writeTarget1(1, value); };
    commandMap[{5, 2}] = [](int value) { writeTarget1(2, value); };
    commandMap[{5, 3}] = [](int value) { writeTarget1(3, value); };
    commandMap[{5, 4}] = [](int value) { writeTarget1(4, value); };
    commandMap[{5, 5}] = [](int value) { writeTarget2(1, value); };
    commandMap[{5, 6}] = [](int value) { writeTarget2(2, value); };
    commandMap[{5, 7}] = [](int value) { writeTarget2(3, value); };
    commandMap[{5, 8}] = [](int value) { writeTarget2(4, value); };
}

// Handle request
void handleRequest(int functionCode, int reg, int value = 0) {
    auto it = commandMap.find({functionCode, reg});
    if (it != commandMap.end()) {
        it->second(value);
    } else {
        std::cout << "Invalid functionCode/register combination" << std::endl;
    }
}

// Main function
int main() {
    setupCommands();

    // Example calls
    handleRequest(3, 1);        // Read from target 1, register 1
    handleRequest(3, 5);        // Read from target 2, register 1
    handleRequest(5, 2, 42);    // Write value 42 to target 1, register 2
    handleRequest(5, 8, 99);    // Write value 99 to target 2, register 4

    return 0;
}
