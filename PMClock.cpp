#include "Target.h"
#include "Frequency.h"
#include "PMClock.h"

// The kernel keeps the time
int _clockYear, _clockMonth, _clockDay, _clockHours, _clockMin, _clockSecs;
long _longCount;

// Adapted from https://stackoverflow.com/questions/7960318/math-to-convert-seconds-since-1970-into-date-and-vice-versa
// Civil is 2019-01-01

int daysFromCivil(int y, int m, int d) {
    y -= m <= 2;
    int era = y / 400;
    int yoe = y - era * 400;
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468 - 17897;
}

int civilYear(int days) {
    days += 719468 + 17897;
    int era = days / 146097;
    int doe = days - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int year = yoe + era * 400;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp = (5 * doy + 2) / 153;
    int month = mp + (mp < 10 ? 3 : -9);
    return year + (month <= 2);
}

int civilMonth(int days) {
    days += 719468 + 17897;
    int era = days / 146097;
    int doe = days - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp = (5 * doy + 2) / 153;
    return mp + (mp < 10 ? 3 : -9);
}

int civilDay(int days) {
    days += 719468 + 17897;
    int era = days / 146097;
    int doe = days - era * 146097;
    int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    int mp = (5 * doy + 2) / 153;
    return doy - (153 * mp + 2) / 5 + 1;
}

int clockYear() {
    return _clockYear;
}

void setClockYear(int year) {
    _clockYear = year;
    writeByteToEEPROM(EPROM_YEAR, year);
}

int clockMonth() {
    return _clockMonth;
}

void setClockMonth(int month) {
    _clockMonth = month;
    writeByteToEEPROM(EPROM_MONTH, month);
}

int clockDay() {
    return _clockDay;
}

void setClockDay(int day) {
    _clockDay = day;
    writeByteToEEPROM(EPROM_DAY, day);
}

int clockHours() {
    return _clockHours;
}

void setClockHours(int hours) {
    _clockHours = hours;
    writeByteToEEPROM(EPROM_HOURS, hours);
}

int clockMins() {
    return _clockMin;
}

void setClockMins(int mins) {
    _clockMin = mins;
    writeByteToEEPROM(EPROM_MINS, mins);
}

int clockSecs() {
    return _clockSecs;
}

long clockEpoch() {
    return _longCount;
}


void setClockSecs(int secs) {
    _clockSecs = secs;
}

frequencyType incrementOneSecond() {
    int daysInMonth = _clockMonth == 2 ? 28 + (_clockYear % 4 == 0 ? 1 : 0) : 31 - (_clockMonth - 1) % 7 % 2;
    _clockSecs++;
    _longCount+=1;
    frequencyType type = second;

    if (_clockSecs == 60) {
        _clockSecs = 0;
        _clockMin++;
        type = minute;
        if (_clockMin % 10 == 0)
            type = minute10;
        if (_clockMin % 15 == 0)
            type = minute15;
        if (_clockMin % 30 == 0)
            type = minute30;
    }
    if (_clockMin == 60) {
        type = hour;
        _clockMin = 0;
        _clockHours++;
        writeByteToEEPROM(EPROM_HOURS, _clockHours);
    }
    if (_clockHours == 24) {
        type = day;
        _clockHours = 0;
        _clockDay++;
        writeByteToEEPROM(EPROM_DAY, _clockDay);
    }
    if (_clockDay == daysInMonth + 1) {
        type = month;
        _clockDay = 1;
        _clockMonth++;
        type = day;
        writeByteToEEPROM(EPROM_MONTH, _clockMonth);
    }
    if (_clockMonth == 13) {
        _clockMonth = 1;
        _clockYear++;
        writeByteToEEPROM(EPROM_YEAR, _clockYear);
    }
    return type;
}

// Seconds since 2019-01-01 00:00:00
unsigned long epochSecs() {
    return daysFromCivil(clockYear() + 2000, clockMonth(), clockDay()) * 86400 + clockHours() * 3600 +
           clockMins() * 60 + clockSecs();
}

void initTimer() {
    _longCount=0;
    _clockYear = readByteFromEEPROM(EPROM_YEAR);
    _clockMonth = readByteFromEEPROM(EPROM_MONTH);
    _clockDay = readByteFromEEPROM(EPROM_DAY);
    _clockHours = readByteFromEEPROM(EPROM_HOURS);
    _clockMin = readByteFromEEPROM(EPROM_MINS);
}
