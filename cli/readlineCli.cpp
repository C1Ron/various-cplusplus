#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

int main() 
{
    std::cout << "Simple CLI (type 'exit' to quit)" << std::endl;
    
    while (true) {
        char* line = readline("> ");
        if (line == nullptr) {
            break;  // Handle Ctrl+D
        }
        std::string command(line);
        if (!command.empty()) {
            add_history(line);  // Add command to history
        }
        free(line);  // readline allocates memory that we need to free
        
        if (command == "exit") {
            break;
        }
        
        std::cout << "You entered: " << command << std::endl;
    }
    
    return 0;
}