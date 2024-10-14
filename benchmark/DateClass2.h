#ifndef DATECLASS2_H
#define DATECLASS2_H

#include <string>
#include <array>
#include <string_view>
#include <cstdint>

class DateClass2
{
public:
    DateClass2();
    DateClass2(const std::string& day, const std::string& month, uint16_t year);
    DateClass2(uint8_t day, uint8_t month, uint16_t year);

    void setDay(const std::string& day);
    void setMonth(const std::string& month);
    void setYear(uint16_t year);
    void setDay(uint8_t day);
    void setMonth(uint8_t month);

    // Original method
    std::string printDateToString() const;

    // New methods for benchmarking
    std::string printDateToString1() const; // Original implementation
    std::string printDateToString2() const; // Preallocated string
    std::string printDateToString3() const; // Fixed-size character array
    std::string printDateToString4() const; // Using string_view

private:
    static constexpr std::array<std::string_view, 7> DAYS = {
        "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"
    };
    static constexpr std::array<std::string_view, 12> MONTHS = {
        "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"
    };

    uint8_t m_day;
    uint8_t m_month;
    uint16_t m_year;

    static uint8_t dayToIndex(const std::string& day);
    static uint8_t monthToIndex(const std::string& month);
};

#endif // DATECLASS2_H