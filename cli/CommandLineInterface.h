#include <iostream>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>

class CommandLineInterface
{
public:
    CommandLineInterface();
    ~CommandLineInterface();
    std::string getCommand();
private:
    std::vector<std::string> history;
    size_t historyIndex;
    termios old_settings;
    void enableRawMode();
    void disableRawMode();
    char getch();
};
