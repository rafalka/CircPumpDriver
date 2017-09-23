/**
 * DateTime.h - General Date and Time handling library
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

#ifndef DATETIME_H_
#define DATETIME_H_

#ifndef __STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>

extern const uint8_t LAST_DAY_OF_MONTH[];
extern const uint16_t HOLIDAYS[];
extern const uint16_t HOLIDAYS_SIZE;

#define MASK_AND_SHIFT_LEFT(_val_,_size_,_offset_) ( ((_val_)&((1<<_size_)-1)) << _offset_)
#define SHIFT_RIGHT_AND_MASK(_val_,_size_,_offset_) ( ((_val_)>>_offset_)&((1<<_size_)-1) )
#define PACK_DATE(_year_,_month_,_day_) ((uint16_t)( MASK_AND_SHIFT_LEFT( (_year_-2000), 7, 9) |  MASK_AND_SHIFT_LEFT(_month_, 4, 5) | MASK_AND_SHIFT_LEFT(_day_, 5, 0) ) )
#define UNPACK_DATE(_hash_,_year_,_month_,_day_) do { \
_year_ = SHIFT_RIGHT_AND_MASK(_hash_, 7, 9)+2000;  _month_ = SHIFT_RIGHT_AND_MASK(_hash_, 4, 5); _day_ = SHIFT_RIGHT_AND_MASK(_hash_, 5, 0); \
} while(0)


typedef struct date_s {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
} dt_date_t;

typedef struct time_s {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} dt_time_t;


class DateTime {

public:

    /**
    Enumerated labels for the days of the week.
    */
    enum WEEK_DAYS {
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY,
        SUNDAY,
        HOLIDAY,
        DAYS_COUNT
    };

    /**
    Enumerated labels for the months.
    */
    enum MONTHS {
        JANUARY=1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER,

        MONTHS_COUNT=DECEMBER
    };

    /** One hour, expressed in seconds */
    static const uint32_t ONE_HOUR = 3600;
    /** One day, expressed in seconds */
    static const uint32_t ONE_DAY = 86400;

    static const uint32_t EPOCH_ERROR = UINT32_MAX;

    static const int8_t  UTC_OFFSET_HOUR_NORM = 1;
    static const int8_t  UTC_OFFSET_HOUR_DST  = 2;

    // Daylight Save Time
    static const uint8_t DST_START_UTC_OFFSET = 2;
    static const uint8_t DST_START_MONTH  = MARCH;
    static const int8_t  DST_START_NTHSUN = -1; // Negative value means nth Sunday from end of the month
    static const uint8_t DST_START_HOUR   = 1;
    static const uint8_t DST_END_MONTH    = OCTOBER;
    static const int8_t  DST_END_NTHSUN   = -1; // Negative value means nth Sunday from end of the month
    static const uint8_t DST_END_HOUR     = 1;
    static const uint8_t WEEKS_IN_MONTH   = 4;
    static const uint8_t DAYS_IN_WEEK     = 7;

    static const char* MONTH_ABBREV[DateTime::MONTHS_COUNT];
    static const char* DAYS_ABBREV[DateTime::DAYS_COUNT];

    static const char* getDayAbbrev(enum WEEK_DAYS day)
    {
        if (day < 0 || day >= DAYS_COUNT) return "BAD";
        return DAYS_ABBREV[day];
    }
    static const char* getMonthAbbrev(enum MONTHS month)
    {
        if (month < JANUARY || month > DECEMBER) return "BAD";
        return MONTH_ABBREV[month - JANUARY ];
    }    
    static bool isLeapYear(uint16_t year)
    {
        return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
    }


    static uint16_t getDaysInYearTillDate(uint8_t month, uint8_t day, bool isLeapYear);
    static bool isDateValid(uint16_t year, uint8_t month, uint8_t day)
    {
        return (year >= 1970) && (year <= 2105) && (month >= 1)
            && (month <= 12) && (day >= 1)
            && (day <= getLastDayOfMonth(month, isLeapYear(year)));
    }
    static bool isDateValid(const dt_date_t* date)
    {
        return (date) && isDateValid(date->year, date->month, date->day);
    }

    static bool areDatesEqual(const dt_date_t* date1, const dt_date_t* date2) {
        return date1 && date2 && (date1->year == date2->year) && (date1->month == date2->month) && (date1->day == date2->day);
    }

    static bool isDateMoreRecent(const dt_date_t* reference, const dt_date_t* date);

    static const char* skipDateSeparators(const char* str);
    static const char* skipToDateSeparators(const char* str);

    static uint8_t getLastDayOfMonth(uint8_t month, bool is_leap_year);
    static uint8_t getMonthFromDateStr(const char* str);
    static uint32_t getUint32FromStr(const char** p_str);
    static uint16_t getYearFromDateStr(const char* str);

    static bool getDateFromStr(const char* str, dt_date_t* date );

    static uint32_t getEpochFromDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);
    static uint32_t getEpochFromDateTime(const dt_date_t* date, const dt_time_t* time);

    static void setDateTimeFromEpoch(uint32_t epoch, dt_date_t* date, dt_time_t* time);

    static enum WEEK_DAYS getWeekDayFromEpoch(uint32_t epoch) {
        return static_cast<WEEK_DAYS>(( (epoch / ONE_DAY) + 3) % 7);
    }

    static enum WEEK_DAYS getDayTypeFromEpoch(uint32_t epoch) {
        return isHoliday(epoch) ? HOLIDAY : getWeekDayFromEpoch(epoch);
    }

    static bool getDatesDelta(const dt_date_t* date1, const dt_date_t* date2, dt_date_t* out_delta);

    static bool isHoliday(uint16_t year, uint8_t  month, uint8_t  day);
    static bool isHoliday(const dt_date_t* date) {
        return date && isHoliday(date->year, date->month, date->day);
    }
    static bool isHoliday(uint32_t epoch) {
        dt_date_t date;
        setDateTimeFromEpoch(epoch, &date, nullptr);
        return isHoliday(&date);
    }

    static bool isTimeValid(uint8_t hour, uint8_t minute, uint8_t second)
    {
        return (hour <= 23) && (minute <= 59) && (second <= 59);
    }
    static bool isTimeValid(const dt_time_t* time)
    {
        return  (time) ? isTimeValid(time->hour, time->minute, time->second) : false;
    }
    static bool areTimesEqual(const dt_time_t* time1, const dt_time_t* time2)
    {
        return (time1) && (time2) && (time1->hour == time2->hour) && (time1->minute== time2->minute) && (time1->second == time2->second);
    }
    
    static bool getTimeFromStr(const char* str, dt_time_t* time);

    static uint32_t getDstEpoch(uint16_t year, uint8_t month, int8_t nth_sunday, uint8_t hour);

    static bool isUtcInDstTime(uint32_t epoch);
    static bool isLocalInDstTime(uint32_t epoch);

    static uint32_t getLocalDateTimeFromUtc(uint32_t epoch)
    {
        return epoch + ONE_HOUR*(isUtcInDstTime(epoch) ? UTC_OFFSET_HOUR_DST : UTC_OFFSET_HOUR_NORM);
    }

    static uint32_t getUtcDateTimeFromLocal(uint32_t epoch)
    {
        return epoch - ONE_HOUR*(isLocalInDstTime(epoch) ? UTC_OFFSET_HOUR_DST : UTC_OFFSET_HOUR_NORM);
    }

};

#endif /* DATETIME_H_ */
