#include <iostream>
#include <string>
#include <sstream>

int main()
{
    std::string str = "Word'1,*02^22 Word-2 ,Word-3 ,Word-4 ,Word-t ,Word-6";
    std::istringstream iss(str);
    std::string token;
    int count = 0;
    while (iss >> token)
    {
        std::cout << token << " ";
        count++;
    }
    std::cout << std::endl;
    std::cout << "Number of words: " << count << std::endl;
    count = 0;

    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ' ') {
            count++;
        }
    }
    std::cout << "Number of spaces: " << count << std::endl;
    return 0;
}