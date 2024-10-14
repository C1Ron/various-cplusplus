#include "DateClass.h"
#include "DateClass2.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    DateClass date1;
    date1.printDate();
    date1.setDay("tuesday");
    date1.setMonth("feb");
    date1.setYear(2022);
    date1.printDate();
    date1.setDay(3);
    date1.setMonth(4);
    date1.printDate();

    DateClass2 date2("sunday", "dec", 2022);
    std::cout << date2.printDateToString() << std::endl;
    date2.setDay("wednesday");
    date2.setMonth("jul");
    date2.setYear(2023);
    std::cout << date2.printDateToString() << std::endl;
    date2.setDay(8);
    date2.setMonth(13);
    std::cout << date2.printDateToString() << std::endl;

    return 0;
}