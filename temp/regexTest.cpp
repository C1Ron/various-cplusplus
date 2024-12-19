#include <iostream>
#include <string>
#include <regex>
/* 
Define the regex pattern using string literals (R"()") to avoid escaping special characters.
The pattern consists of 3 capture groups: the register type, the register number, and the value.
    - register type is either "FEC_RT_REG" or "FOC_REG".
    - register number and value are integers
    - the "Value:" pattern is used to separate the register number and value.
    - the \s* pattern matches zero or more whitespace characters.
    - the \d+ pattern matches one or more digits.
    - capture groups are enclosed in parentheses.

The std::smatch class is used to store the results of the regex search.
    - matches[0] contains the entire match.
    - matches[1] contains the first capture group.
    - matches[2] contains the second capture group.
    - matches[3] contains the third capture group.

The std::stoi function converts strings to integers.
*/

void extractParameters(const std::string& input, std::string& registerType, int& reg, int& value) {
    //std::regex regexPattern(R"((FEC_RT_REG|FOC_REG):\s*(\d+)\s*Value:\s*(\d+))");
    std::regex regexPattern(R"((FEC_RT_REG|FOC_REG):\s*(\d+)\s*Value:\s*(-?\d+))");

    std::smatch matches;
    
    if (std::regex_search(input, matches, regexPattern)) {
        if (matches.size() == 4) {
            registerType = matches[1].str();
            reg = std::stoi(matches[2].str()); 
            value = std::stoi(matches[3].str());
        }
    }
}

int main() {
    std::string input1 = "\033[1;92m\tFEC_RT_REG:\t4\tValue:\t50\n\033[0m";
    std::string input2 = "\033[1;92m\tmsc_id: (0x1)\tFOC_REG:\t9\tValue:\t21 (0x15)\t0000000000010101\n\033[0m";
    std::string input3 = "\033[1;92m\t\t\tFEC_RT_REG:\t3\tValue:\t-212\n\033[0m";

    std::string registerType;
    int reg = 0;
    int value = 0;

    extractParameters(input1, registerType, reg, value);
    std::cout << "Input 1:\nRegister Type: " << registerType << "\nRegister: " << reg << "\nValue: " << value << "\n\n";

    extractParameters(input2, registerType, reg, value);
    std::cout << "Input 2:\nRegister Type: " << registerType << "\nRegister: " << reg << "\nValue: " << value << "\n\n";

    extractParameters(input3, registerType, reg, value);
    std::cout << "Input 3:\nRegister Type: " << registerType << "\nRegister: " << reg << "\nValue: " << value << "\n";
    
    return 0;
}