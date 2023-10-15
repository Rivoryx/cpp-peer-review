#include <array>
#include <stdexcept>

enum class TimeUnit {
    Hour,
    Minute,
    Second
};

// Проверка, является ли указанный год допустимым
void CheckYearValidity(int year) {
    if (year < 1 || year > 9999) {
        throw std::domain_error("Year is out of range");
    }
}

// Проверка, является ли указанный месяц допустимым
void CheckMonthValidity(int month) {
    if (month < 1 || month > 12) {
        throw std::domain_error("Month is out of range");
    }
}

// Проверка, является ли указанный день допустимым для указанного года и месяца
void CheckDayValidity(int year, int month, int day) {
    const bool isLeapYear = (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
    const std::array<int, 12> daysInMonth = { 31, 28 + isLeapYear, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (day < 1 || day > daysInMonth[month - 1]) {
        throw std::domain_error("Day is out of range for the given year and month");
    }
}

// Проверка, является ли указанный час, минута или секунда допустимым значением
void CheckTimeUnitValidity(int value, TimeUnit unit) {
    int maxValue = 0;
    switch (unit) {
    case TimeUnit::Hour:
        maxValue = 23;
        break;
    case TimeUnit::Minute:
        maxValue = 59;
        break;
    case TimeUnit::Second:
        maxValue = 59;
        break;
    }

    if (value < 0 || value > maxValue) {
        throw std::domain_error("Time unit is out of range");
    }
}

// Проверка допустимости всей структуры DateTime
void CheckDateTimeValidity(const DateTime& dt) {
    CheckYearValidity(dt.year);
    CheckMonthValidity(dt.month);
    CheckDayValidity(dt.year, dt.month, dt.day);
    CheckTimeUnitValidity(dt.hour, TimeUnit::Hour);
    CheckTimeUnitValidity(dt.minute, TimeUnit::Minute);
    CheckTimeUnitValidity(dt.second, TimeUnit::Second);
}
