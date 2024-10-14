#ifndef DATECLASS_H
#define DATECLASS_H
#include <string>
#include <iostream>
#include <unordered_map>

class DateClass
{
public:
    DateClass();
    DateClass(std::string day, std::string month, uint year);
    DateClass(uint day, uint month, uint year);
    ~DateClass();
    void printDate() const;
    void setDay(std::string day);
    void setMonth(std::string month);
    void setYear(uint year);
    void setDay(uint day);
    void setMonth(uint month);
    std::string printDateToString() const;
private:
    std::unordered_map<uint, std::string> dayMap;
    std::unordered_map<uint, std::string> monthMap;

    std::string sday;
    std::string smonth;
    uint uday;
    uint umonth;
    uint year;

    void initializeMaps();
};

#endif // DATECLASS_H