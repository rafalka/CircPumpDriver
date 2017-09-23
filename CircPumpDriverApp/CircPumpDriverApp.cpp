// CircPumpDriverApp.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "DateTime.h"
#include "CircShedule.h"
#include <time.h>
#include <iomanip>

using namespace std;



const char* separator = "\n------------------------------------------------------------------------------";
void checkHoliday(uint16_t year, uint8_t  month, uint8_t  day)
{
    cout << "Date: " << year << "-" << month << "-" << day <<
        (DateTime::isHoliday(year, month, day) ? " is" : " is NOT ")
        << " a holiday" << endl;
}


// Returns number of days since civil 1970-01-01.  Negative values indicate
//    days prior to 1970-01-01.
// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
//                 m is in [1, 12]
//                 d is in [1, last_day_of_month(y, m)]
//                 y is "approximately" in
//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
//                 Exact range of validity is:
//                 [civil_from_days(numeric_limits<Int>::min()),
//                  civil_from_days(numeric_limits<Int>::max()-719468)]
template <class Int>
constexpr
Int
days_from_civil(Int y, unsigned m, unsigned d) noexcept
{
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
        "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
        "This algorithm has not been ported to a 16 bit signed integer");
    y -= m <= 2;
    const Int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;         // [0, 146096]
    return era * 146097 + static_cast<Int>(doe) - 719468;
}

// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
template <class Int>
constexpr
std::tuple<Int, unsigned, unsigned>
civil_from_days(Int z) noexcept
{
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
        "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
        "This algorithm has not been ported to a 16 bit signed integer");
    z += 719468;
    const Int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;  // [0, 399]
    const Int y = static_cast<Int>(yoe) + era * 400;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);                // [0, 365]
    const unsigned mp = (5 * doy + 2) / 153;                                   // [0, 11]
    const unsigned d = doy - (153 * mp + 2) / 5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    return std::tuple<Int, unsigned, unsigned>(y + (m <= 2), m, d);
}

// Returns day of week in civil calendar [0, 6] -> [Sun, Sat]
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-4].

constexpr
uint8_t
weekday_from_days(uint32_t timestamp) noexcept
{
    return static_cast<uint8_t>((timestamp + 3) % 7);
}




/* Set the tm_t fields for the local time. */

void checkHolidays()
{
    uint16_t year; uint8_t  month; uint8_t  day;
    uint16_t hash;
    for (int i = 0; i < HOLIDAYS_SIZE; i++)
    {
        hash = HOLIDAYS[i];
        UNPACK_DATE(hash, year, month, day);
       if (!DateTime::isHoliday(year, month, day) )
       {
           cout << "Date: " << year << "-" << month << "-" << day << " not handled!" << endl;
       }
    }
    cout << "checkHolidays done!" << endl;
}
void DisplayDate(uint16_t year, uint8_t month, uint8_t day)
{
    cout << "Date: " << year << "-" << setw(2) << setfill('0') <<  static_cast<unsigned>(month) << "-" << static_cast<unsigned>(day) << endl;
}
void DisplayDate(const dt_date_t& date)
{
    DisplayDate(date.year, date.month, date.day);
}

void DisplayTime(uint16_t hour, uint8_t minute, uint8_t second)
{
    cout << "Time: "  << setw(2) << setfill('0') << static_cast<unsigned>(hour) << ":" << static_cast<unsigned>(minute) << ":" << static_cast<unsigned>(second) << endl;
}
void DisplayTime(const dt_time_t& time)
{
    DisplayTime(time.hour, time.minute, time.second);
}

void DisplayDateTime(uint32_t epoch)
{
    if (epoch==DateTime::EPOCH_ERROR)
    {
        printf("Epoch ERROR!!!\n");
        return;
    }
    dt_date_t date;
    dt_time_t time;
    DateTime::setDateTimeFromEpoch(epoch, &date, &time);
    printf("%d: %.4d-%s-%.2d, %s, %.2d:%.2d:%.2d\n",epoch,
        date.year,DateTime::getMonthAbbrev(static_cast<DateTime::MONTHS>(date.month)),date.day,
        DateTime::getDayAbbrev(DateTime::getDayTypeFromEpoch(epoch)),
            time.hour,time.minute,time.second
        );
}

void CheckDstConverstion(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t epoch;
    epoch = DateTime::getEpochFromDateTime(year,month,day,hour,min,sec);
    printf(DateTime::isUtcInDstTime(epoch) ? "DST:   " : "Reg:  "); DisplayDateTime(epoch);
    epoch = DateTime::getLocalDateTimeFromUtc(epoch);
    printf("Local:   ");  DisplayDateTime(epoch);
    epoch = DateTime::getUtcDateTimeFromLocal(epoch);
    printf("UTC:     ");  DisplayDateTime(epoch);

}


const circ_shedule_table_t shedule_table = {
    /* Monday */{ { CT(3,55, 0), CT(4,55, 0) },{ CT(6,00 , 0), CT(8,35, 0) },{ CT(16,40, 0), CT(22,40, 0) }, },
    /* Tuesday */{ { CT(3,55, 0), CT(4,55, 0) },{ CT(16,40 , 0), CT(22,40, 0) }, },
    /* Wednesday */{ { CT(23,55, 0), CT(24, 0, 0) }, },
    /* Thursay */{ { CT(3,55, 0), CT(4,55, 0) },{ CT(16,40 , 0), CT(23,59,59) }, },
    /* Friday */{ { CT(0, 0, 0), CT(1, 0, 0) },{ CT(3,55 , 0), CT(4,55, 0) },{ CT(7, 0, 0), CT(23,59,59) }, },
    /* Saturday */{ { CT(0, 0, 0), CT(2, 0, 0) },{ CT(8,00 , 0), CT(23,59,59) }, },
    /* Sunday */{ { CT(0, 0, 0), CT(4, 0, 0) },{ CT(8,00 , 0), CT(23,59,59) }, },
    /* Holiday */{ { CT(0, 0, 0), CT(2, 0, 0) },{ CT(8,00 , 0), CT(23,59,59) }, }
};
void DisplayNextCircOnTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint32_t epoch;
    epoch = DateTime::getEpochFromDateTime(year, month, day, hour, min, sec);
    puts(separator);
    printf("Current:    ");  DisplayDateTime(epoch);
    epoch = CircShedule::getNextOnTime(&shedule_table, epoch);
    printf("  Next On:  ");  DisplayDateTime(epoch);
/*	uint32_t utc = DateTime::getUtcDateTimeFromLocal(epoch);
    utc += 120;
    epoch = DateTime::getLocalDateTimeFromUtc(utc)*/;
    printf("  Next Off: ");  DisplayDateTime(epoch+120);
    printf("  UTC On:   ");  DisplayDateTime(DateTime::getUtcDateTimeFromLocal(epoch));
    printf("  UTC Off:  ");  DisplayDateTime(DateTime::getUtcDateTimeFromLocal(epoch+120));
}

extern void start_simulation();
int main()
{
    //checkHolidays();

    start_simulation();

    dt_date_t date;
    dt_time_t time;
    uint32_t epoch;

    DisplayNextCircOnTime(2014, 10, 26, 1, 50, 0);
    DisplayNextCircOnTime(2014, 10, 26, 1, 59, 0);
    DisplayNextCircOnTime(2014, 10, 26, 2, 59, 0);

    DisplayNextCircOnTime(2014, 3, 30, 1, 50, 0);
    DisplayNextCircOnTime(2014, 3, 30, 1, 59, 0);
    DisplayNextCircOnTime(2014, 3, 30, 2, 50, 0);
    DisplayNextCircOnTime(2014, 3, 30, 2, 59, 0);
    DisplayNextCircOnTime(2014, 3, 30, 3, 50, 0);

    /*
    DisplayDateTime(DateTime::getDstEpoch(2012, 3, -1, 2));
    CheckDstConverstion(2012, 3, 25, 1, 0, 0);
    CheckDstConverstion(2012, 10, 28, 0, 59, 59);
    CheckDstConverstion(2012, 10, 28, 1, 0, 0);
    DisplayDateTime(DateTime::getDstEpoch(2013, 3, -1, 2));
    DisplayDateTime(DateTime::getDstEpoch(2014, 3, -1, 2));
    DisplayDateTime(DateTime::getDstEpoch(2015, 3, -1, 2));
    DisplayDateTime(DateTime::getDstEpoch(2016, 3, -1, 2));
    DisplayDateTime(DateTime::getDstEpoch(2017, 3, -1, 2));

    DisplayDateTime(DateTime::getDstEpoch(2012, 10, -1, 3));
    DisplayDateTime(DateTime::getDstEpoch(2013, 10, -1, 3));
    DisplayDateTime(DateTime::getDstEpoch(2014, 10, -1, 3));
    DisplayDateTime(DateTime::getDstEpoch(2015, 10, -1, 3));
    DisplayDateTime(DateTime::getDstEpoch(2016, 10, -1, 3));
    DisplayDateTime(DateTime::getDstEpoch(2017, 10, -1, 3));

    DateTime::getDateFromStr(__DATE__, &date);
    DateTime::getTimeFromStr(__TIME__, &time);

    //DateTime::setDateTimeFromEpoch(epoch, &date, &time);
    DisplayDate(date);
    DisplayTime(time);

    uint32_t epoch = DateTime::getEpochFromDateTime(&date, &time);
    cout << epoch << endl;
    cout << DateTime::getWeekDayFromEpoch(epoch) << endl;
    cout << DateTime::getDayAbbrev(DateTime::getWeekDayFromEpoch(DateTime::getEpochFromDateTime(2023, 5, 1, 0, 0, 0))) << endl;
    cout << DateTime::getDayAbbrev(DateTime::getWeekDayFromEpoch(DateTime::getEpochFromDateTime(2024, 11, 1, 0, 0, 0))) << endl;
    */
    return 0;
}

