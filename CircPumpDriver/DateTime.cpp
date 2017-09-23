/**
 * DateTime.cpp - General Date and Time handling library
 * with support to Local Time and Hollidays table
 *
 * Copyright (c) 2017, Rafal Kukla. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * - Neither the name of the "Rafal Kukla" nor the names of its contributors may be used to endorse or
 *   promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 */

#include "DateTime.h"


const uint8_t  LAST_DAY_OF_MONTH[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
const uint16_t DAYS_UP_TO_MONTH_REGULAR_YEAR[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
const uint16_t DAYS_UP_TO_MONTH_LEAP_YEAR[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };


const char* DateTime::MONTH_ABBREV[DateTime::MONTHS_COUNT] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const char* DateTime::DAYS_ABBREV[DateTime::DAYS_COUNT] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Hol"};

const uint16_t HOLIDAYS[] = {
#include "Holidays_2017-2030.inc"
};

const uint16_t HOLIDAYS_SIZE = sizeof(HOLIDAYS)/sizeof(HOLIDAYS[0]);


uint32_t DateTime::getUint32FromStr(const char** p_str) {

    if (!p_str) return 0;
    const char* str = *p_str;
    uint32_t val = 0;
    char c;

    while (*str == ' ') str++;
    for (;; str++)
    {
        c = *str;
        if (c >= '0' && c <= '9')
        {
            val *= 10;
            val += c - '0';
        }
        else
        {
            break;
        }
    }
    *p_str = str;
    return val;
}

const char* DateTime::skipDateSeparators(const char* str)
{
    for(;;)
    {
        switch (*str)
        {
        case ' ':
        case '-':
        case '.':
        case ',':
        case '/':
            str++;
            continue;
        case 0:
            return nullptr;
        default:
            return str;
        }
    }
}

const char* DateTime::skipToDateSeparators(const char* str) {
    for (;;)
    {
        switch (*str)
        {
        case ' ':
        case '-':
        case '.':
        case ',':
        case '/':
            return str;
        case 0:
            return nullptr;
        default:
            str++;
        }
    }
}

uint8_t DateTime::getLastDayOfMonth(uint8_t month, bool is_leap_year)
{
    if ((month < 1) || (month) > 12) return 0;
    if ((month == 2) && is_leap_year) return 29;
    return LAST_DAY_OF_MONTH[month - 1];
}


char to_upper(char c)
{
    if (c >= 'a' && c <= 'z') c = 'A' + (c - 'a');
    return c;
}
#define get_month_hash(c1, c2, c3) ( MASK_AND_SHIFT_LEFT(c1-'A',5,10) | MASK_AND_SHIFT_LEFT(c2-'A',5,5) | MASK_AND_SHIFT_LEFT(c3-'A',5,0) )
uint8_t DateTime::getMonthFromDateStr(const char* str)
{
    if (!str[0] || !str[1] || !str[2]) return 0;

    switch (get_month_hash(to_upper(str[0]), to_upper(str[1]), to_upper(str[2]) ) )
    {
        case get_month_hash('J', 'A', 'N'): return 1;
        case get_month_hash('F', 'E', 'B'): return 2;
        case get_month_hash('M', 'A', 'R'): return 3;
        case get_month_hash('A', 'P', 'R'): return 4;
        case get_month_hash('M', 'A', 'Y'): return 5;
        case get_month_hash('J', 'U', 'N'): return 6;
        case get_month_hash('J', 'U', 'L'): return 7;
        case get_month_hash('A', 'U', 'G'): return 8;
        case get_month_hash('S', 'E', 'P'): return 9;
        case get_month_hash('O', 'C', 'T'): return 10;
        case get_month_hash('N', 'O', 'V'): return 11;
        case get_month_hash('D', 'E', 'C'): return 12;
        default:
            return 0;
    }
}

uint16_t DateTime::getYearFromDateStr(const char* str) {
    return static_cast<uint16_t>( getUint32FromStr(&str) );
}



bool DateTime::isDateMoreRecent(const dt_date_t* reference, const dt_date_t* date)
{
    {
        if (!reference || !date) return false;
        if 	(date->year < reference->year) return false;
        if 	(date->year > reference->year) return true;

        if 	(date->month < reference->month) return false;
        if 	(date->month > reference->month) return true;

        if 	(date->day > reference->day) return true;
        return false;
    }
}

bool DateTime::getDateFromStr(const char* str, dt_date_t* out_date )
{
    dt_date_t date = {0,0,0};

    uint16_t val;
    bool result;
    while ( (str = skipDateSeparators(str)) )
    {
        val = getMonthFromDateStr(str);
        if (val) {
            date.month = static_cast<uint8_t>(val);
        }
        else {
            val = getUint32FromStr(&str);
            if (!val)
            {
                return false;
            }
            if (val > 31) date.year = val; else date.day = static_cast<uint8_t>(val);
        }

        if (!((str = skipToDateSeparators(str))) ) break;

    }
    result = isDateValid(&date);

    if (result && out_date ) {
        *out_date = date;
    }
    return result;
}

bool DateTime::getDatesDelta(const dt_date_t* start_date, const dt_date_t* end_date,
        dt_date_t* out_delta)
{
    if (!start_date || !end_date) return false;

    if (areDatesEqual(start_date,end_date)) {
        if (out_delta) {
            out_delta->year = out_delta->month = out_delta->day = 0;
            return true;
        }
    }

    if (!isDateMoreRecent(start_date, end_date)) return false;

    dt_date_t delta;
    dt_date_t end = *end_date;


    if (end.day >= start_date->day) {
        delta.day = end.day -  start_date->day;
    } else {
        delta.day = getLastDayOfMonth(start_date->month,isLeapYear(start_date->year)) - start_date->day + end.day;
        end.month--;
    }
    if (end.month >= start_date->month) {
        delta.month = end.month - start_date->month;
    } else {
        delta.month = 12-start_date->month+ end.month;
        end.year--;
    }
    delta.year = end.year - start_date->year;

    if (out_delta) {
        *out_delta = delta;
    }

    return true;
}


uint16_t DateTime::getDaysInYearTillDate(uint8_t month, uint8_t day, bool isLeapYear)
{
    month--;

    return ( (isLeapYear) ?
        DAYS_UP_TO_MONTH_LEAP_YEAR[month] :
        DAYS_UP_TO_MONTH_REGULAR_YEAR[month] ) + day - 1;
}

uint32_t DateTime::getEpochFromDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if (!isDateValid(year, month, day) || !isTimeValid(hour, min, sec)) return EPOCH_ERROR;

    uint16_t tm_yday = getDaysInYearTillDate(month, day, isLeapYear(year));

    year -= 1900;

    return sec + min * 60UL + hour * 3600UL + tm_yday * 86400UL +
        (year - 70U) * 31536000UL + ((year - 69U) / 4) * 86400UL -
        ((year - 1UL) / 100UL) * 86400UL + ((year + 299UL) / 400UL) * 86400UL;
}

uint32_t DateTime::getEpochFromDateTime(const dt_date_t* date, const dt_time_t* time)
{
    dt_date_t d;
    dt_time_t t;
    
    if (date) d = *date; else d = { 1970, 1, 1 };
    if (time) t = *time; else t = { 0, 0, 0 };

    return getEpochFromDateTime( d.year, d.month, d.day, t.hour, t.minute, t.second);
}

void DateTime::setDateTimeFromEpoch(uint32_t epoch, dt_date_t* date, dt_time_t* time)
{
    register uint32_t year;
    register uint32_t month, day, hour, mins, secs;
    unsigned leap;
    day = epoch / (24L * 60 * 60);
    secs = epoch % (24L * 60 * 60);
    mins = secs / 60;
    secs %= 60;
    hour = mins / 60;
    mins %= 60;
    year = (((day * 4U) + 2) / 1461U);
    day -= ((year * 1461U) + 1) / 4;
    year += 1970U;
    leap = isLeapYear(year) ? 1: 0;
    day += (day > 58U + leap) ? ((leap) ? 1 : 2) : 0;
    month = ((day * 12) + 6) / 367;
    day += 1 - ((month * 367U) + 5) / 12;
    month++;
    if (date) {
        date->year = year;
        date->month = month;
        date->day = day;
    }
    if (time) {
        time->hour = hour;
        time->minute = mins;
        time->second = secs;
    }
}


bool DateTime::isHoliday(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t hash = PACK_DATE(year, month, day);
    
    if (hash<HOLIDAYS[0] && hash>HOLIDAYS[HOLIDAYS_SIZE-1]) return false;

    int start = 0,  end = HOLIDAYS_SIZE - 1, mid;
    while (start<end)
    {
        mid = (end - start) / 2;
        if (hash == HOLIDAYS[mid]) return true;
        
        if (hash < HOLIDAYS[mid])  {
            if (hash == HOLIDAYS[start]) return true;
            end = mid-1;
            start++;
        }  else  {
            if (hash == HOLIDAYS[end]) return true;
            start = mid+1;
            end--;
        }
    }
    return false;
}


bool DateTime::getTimeFromStr(const char* str, dt_time_t* time)
{
    uint8_t fields[3];
    uint32_t val;

    for (unsigned idx=0; idx<sizeof(fields)/sizeof(fields[0]); idx++)
    {
        while (*str == ' ' || *str == ':') str++;
        if (!*str) return false;
        val = getUint32FromStr(&str);
        if (val > UINT8_MAX) return false;
        fields[idx] = static_cast<uint8_t>(val);
    }
    if (!isTimeValid(fields[0], fields[1], fields[2])) return false;

    if (time)
    {
        time->hour = fields[0];
        time->minute = fields[1];
        time->second = fields[2];
    }
    return true;
}

uint32_t DateTime::getDstEpoch(uint16_t year, uint8_t month, int8_t nth_sunday, uint8_t hour)
{
    if (!nth_sunday || nth_sunday > WEEKS_IN_MONTH || nth_sunday<-WEEKS_IN_MONTH) return EPOCH_ERROR;

    uint8_t day = (nth_sunday > 0) ? 1 : getLastDayOfMonth(month, isLeapYear(year));
    uint32_t epoch = getEpochFromDateTime(year, month, day, hour, 0, 0);
    if (epoch == EPOCH_ERROR) return epoch;

    day = getWeekDayFromEpoch(epoch);

    return (nth_sunday > 0)
        ? epoch + (SUNDAY - day + DAYS_IN_WEEK*(nth_sunday - 1)) * ONE_DAY
        : epoch - ((day + 1) % DAYS_IN_WEEK + DAYS_IN_WEEK*(nth_sunday + 1)) * ONE_DAY;
}

bool DateTime::isUtcInDstTime(uint32_t epoch)
{
    dt_date_t date;
    setDateTimeFromEpoch(epoch, &date, nullptr);
    uint32_t start = getDstEpoch(date.year, DST_START_MONTH, DST_START_NTHSUN, DST_START_HOUR);
    uint32_t end   = getDstEpoch(date.year, DST_END_MONTH,   DST_END_NTHSUN,   DST_END_HOUR);

    return (epoch >= start && epoch < end);
}

bool DateTime::isLocalInDstTime(uint32_t epoch)
{
    dt_date_t date;
    setDateTimeFromEpoch(epoch, &date, nullptr);
    uint32_t start = getDstEpoch(date.year, DST_START_MONTH, DST_START_NTHSUN, DST_START_HOUR + UTC_OFFSET_HOUR_NORM);
    uint32_t end   = getDstEpoch(date.year, DST_END_MONTH, DST_END_NTHSUN, DST_END_HOUR + UTC_OFFSET_HOUR_DST);

    return (epoch >= start && epoch < end);
}
