#include "DateClass2.h"
#include <iostream>
#include <algorithm>
#include <string.h>
DateClass2::DateClass2() : m_day(0), m_month(0), m_year(2021) 
{
    // Empty
}

DateClass2::DateClass2(const std::string& day, const std::string& month, uint16_t year)
    : m_day(dayToIndex(day)), m_month(monthToIndex(month)), m_year(year) 
{
    // Empty
}

DateClass2::DateClass2(uint8_t day, uint8_t month, uint16_t year)
    : m_day(day > 0 && day <= 7 ? day - 1 : 0),
      m_month(month > 0 && month <= 12 ? month - 1 : 0),
      m_year(year>0 ? year : 2021) 
{
    // Empty
}

void DateClass2::setDay(const std::string& day) 
{
    m_day = dayToIndex(day);
}

void DateClass2::setMonth(const std::string& month) 
{
    m_month = monthToIndex(month);
}

void DateClass2::setYear(uint16_t year) 
{
    m_year = year;
}

void DateClass2::setDay(uint8_t day) 
{
    m_day = (day > 0 && day <= 7) ? day - 1 : m_day;
}

void DateClass2::setMonth(uint8_t month) 
{
    m_month = (month > 0 && month <= 12) ? month - 1 : m_month;
}

uint8_t DateClass2::dayToIndex(const std::string& day) 
{
    auto it = std::find(DAYS.begin(), DAYS.end(), day);
    return it != DAYS.end() ? std::distance(DAYS.begin(), it) : 0;
}

uint8_t DateClass2::monthToIndex(const std::string& month) 
{
    auto it = std::find(MONTHS.begin(), MONTHS.end(), month);
    return it != MONTHS.end() ? std::distance(MONTHS.begin(), it) : 0;
}

std::string DateClass2::printDateToString() const 
{
    return std::string(DAYS[m_day]) + std::string(", ") + std::string(MONTHS[m_month]) + std::string(" ") + std::to_string(m_year);
}
// Original method
std::string DateClass2::printDateToString1() const
{
    return this->printDateToString();
}
// Method 2: Preallocated string
std::string DateClass2::printDateToString2() const {
    std::string result;
    result.reserve(20);  // Preallocate space for the result
    result += DAYS[m_day];
    result += ", ";
    result += MONTHS[m_month];
    result += " ";
    result += std::to_string(m_year);
    return result;
}

// Method 3: Fixed-size character array
std::string DateClass2::printDateToString3() const {
    char result[20];
    int offset = 0;
    memcpy(result + offset, DAYS[m_day].data(), DAYS[m_day].size());
    offset += DAYS[m_day].size();
    memcpy(result + offset, ", ", 2);
    offset += 2;
    memcpy(result + offset, MONTHS[m_month].data(), MONTHS[m_month].size());
    offset += MONTHS[m_month].size();
    result[offset++] = ' ';
    offset += sprintf(result + offset, "%d", m_year);
    return std::string(result, offset);
}

// Method 4: Using string_view (similar to Method 2 but with string_view)
std::string DateClass2::printDateToString4() const {
    std::string result;
    result.reserve(20);
    result += DAYS[m_day];
    result += ", ";
    result += MONTHS[m_month];
    result += " ";
    result += std::to_string(m_year);
    return result;
}

