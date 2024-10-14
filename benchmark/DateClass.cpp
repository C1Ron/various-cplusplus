#include "DateClass.h"

DateClass::DateClass() : sday("monday"), smonth("jan"), year(2021), uday(1), umonth(1)
{
    initializeMaps();
}

DateClass::DateClass(std::string day, std::string month, uint year) : sday(day), smonth(month), year(year)
{
    initializeMaps();
    for (auto &d : dayMap) {
        if (d.second == day) {
            uday = d.first;
            break;
        }
    }
    for (auto &m : monthMap) {
        if (m.second == month) {
            umonth = m.first;
            break;
        }
    }
}

DateClass::DateClass(uint day, uint month, uint year) : uday(day), umonth(month), year(year) 
{
    initializeMaps();
    if (dayMap.find(day) != dayMap.end()) {
        sday = dayMap[day];
    }
    if (monthMap.find(month) != monthMap.end()) {
        smonth = monthMap[month];
    }
}

DateClass::~DateClass() {}

void DateClass::initializeMaps() 
{
    dayMap = {
        {1, "monday"}, {2, "tuesday"}, {3, "wednesday"},
        {4, "thursday"}, {5, "friday"}, {6, "saturday"}, {7, "sunday"}
    };
    monthMap = {
        {1, "jan"}, {2, "feb"}, {3, "mar"}, {4, "apr"},
        {5, "may"}, {6, "jun"}, {7, "jul"}, {8, "aug"},
        {9, "sep"}, {10, "oct"}, {11, "nov"}, {12, "dec"}
    };
}

void DateClass::printDate() const {
    std::cout << sday << ", " << smonth << " " << year << std::endl;
}

void DateClass::setDay(std::string day) 
{
    for (auto &d : dayMap) {
        if (d.second == day) {
            uday = d.first;
            sday = day;
            break;
        }
    }
    
}

void DateClass::setMonth(std::string month) 
{
    for (auto &m : monthMap) {
        if (m.second == month) {
            umonth = m.first;
            smonth = month;
            break;
        }
    }
}

void DateClass::setYear(uint year) 
{
    this->year = year;
}

void DateClass::setDay(uint day) 
{
    if (dayMap.find(day) != dayMap.end()) {
        sday = dayMap[day];
        uday = day;
    }
}

void DateClass::setMonth(uint month) {
    if (monthMap.find(month) != monthMap.end()) {
        smonth = monthMap[month];
        umonth = month;
    }
}

std::string DateClass::printDateToString() const 
{
    return sday + ", " + smonth + " " + std::to_string(year);
}