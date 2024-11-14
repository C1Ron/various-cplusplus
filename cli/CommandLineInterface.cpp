#include "CommandLineInterface.h"

CommandLineInterface::CommandLineInterface() : historyIndex(0)
{
    enableRawMode();
}

CommandLineInterface::~CommandLineInterface()
{
    disableRawMode();
}

std::string CommandLineInterface::getCommand()
{
    std::string command;
    size_t cursorPos = 0;

    while (true) {
        char c = getch();
        
        // Handle special keys
        if (c == '\033') {  // Escape sequence for arrow keys
            char next = getch();
            if (next == '[') {
                char arrow = getch();
                if (arrow == 'A') {  // Up arrow
                    if (!history.empty() && historyIndex > 0) {
                        // Clear current line
                        while (cursorPos > 0) {
                            std::cout << "\b \b";
                            cursorPos--;
                        }
                        historyIndex--;
                        command = history[historyIndex];
                        std::cout << command;
                        std::cout.flush();
                        cursorPos = command.length();
                    }
                }
                else if (arrow == 'B') {  // Down arrow
                    // Clear current line
                    while (cursorPos > 0) {
                        std::cout << "\b \b";
                        cursorPos--;
                    }
                    if (historyIndex < history.size() - 1) {
                        historyIndex++;
                        command = history[historyIndex];
                    } else {
                        historyIndex = history.size();
                        command = "";
                    }
                    std::cout << command;
                    std::cout.flush();
                    cursorPos = command.length();
                }
            }
        }
        else if (c == '\n' || c == '\r') {  // Enter key
            std::cout << std::endl;
            if (!command.empty()) {
                history.push_back(command);
                historyIndex = history.size();
            }
            return command;
        }
        else if (c == 127 || c == 8) {  // Backspace
            if (cursorPos > 0) {
                command.erase(--cursorPos, 1);
                std::cout << "\b \b";
                std::cout.flush();
            }
        }
        else if (c >= 32 && c <= 126) {  // Printable characters
            command.insert(cursorPos++, 1, c);
            std::cout << c;
            std::cout.flush();
        }
    }
}

void CommandLineInterface::enableRawMode()
{
    termios new_settings;
    tcgetattr(STDIN_FILENO, &old_settings);
    new_settings = old_settings;
    new_settings.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
}

void CommandLineInterface::disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
}

char CommandLineInterface::getch()
{
    char c;
    read(STDIN_FILENO, &c, 1);
    return c;
}