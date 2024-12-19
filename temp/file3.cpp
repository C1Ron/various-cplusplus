#include <iostream>
#include <csignal>
#include <unistd.h>

/*
There are many types of signals that can be caught by a process. For example:
- SIGINT: 2     (CTRL+C)
- SIGTERM: 15   (kill -TERM $(pidof file3))
Other signals cannot be caught by a process. For example:
- SIGKILL: 9    (kill -KILL $(pidof file3))
- SIGSTOP: 19   (CTRL+Z)

You can send signals to a process uing the kill command.

Some signals can be overridden. For example:
- SIGTSTP: 20   (CTRL+Z)
This override enables us to catch CTRL+Z.
*/

void s2Handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    std::cout << "Returning from SIGINT handler" << std::endl;
}

void s15Handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    std::cout << "Returning from SIGTERM handler" << std::endl;
}

void s20Handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    std::cout << "Returning from SIGTSTP handler" << std::endl;
}

int main()
{
    signal(SIGINT, s2Handler);      // kill -INT $(pidof file3) or CTRL+C
    signal(SIGTERM, s15Handler);    // kill -TERM $(pidof file3)
    signal(SIGTSTP, s20Handler);    // Will override default behaviour of  CTRL+Z

    int count = 0;
    while(count < 100)
    {
        std::cout << "Running..." << std::endl;
        sleep(1);
        count++;
    }
}