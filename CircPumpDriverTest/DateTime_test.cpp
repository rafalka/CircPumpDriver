/*
 * DaateTime_test.cpp
 *
 *  Created on: 29 gru 2016
 *      Author: ark036
 */
#include <stdio.h>
#include <gtest/gtest.h>
#include "DateTime.h"


TEST(simple,positive)
{
	printf("%s: %s,%s\n",__func__,__DATE__,__TIME__);
}

TEST(Check_isLeapYear,positive)
{
	ASSERT_TRUE(DateTime::isLeapYear(2016));
}

TEST(Check_isLeapYear,negative)
{
	ASSERT_FALSE(DateTime::isLeapYear(2015));
}

TEST(Check_get_month_from_date,positive)
{
	const char* test_names[] = {
		"123", //Fail case
		"Jan","feb","MAR","apr","maY","JUN","jUl","auG","SeP","oCt","nov","DEC"
	};

	for (unsigned month=0;month<(sizeof(test_names)/sizeof(*test_names)); month++)
	{
		ASSERT_EQ(DateTime::getMonthFromDateStr(test_names[month]),month);
	}
}

TEST(Check_get_year_from_date,positive)
{
	ASSERT_EQ(DateTime::getYearFromDateStr(" 2016 "),2016);
	ASSERT_EQ(DateTime::getYearFromDateStr(" 0001 "),1);
	ASSERT_EQ(DateTime::getYearFromDateStr(" 65535 "),65535);
}

TEST(Check_getLastDayOfMonth,positive)
{
	ASSERT_EQ(DateTime::getLastDayOfMonth(1,true),31);
	ASSERT_EQ(DateTime::getLastDayOfMonth(1,false),31);
	ASSERT_EQ(DateTime::getLastDayOfMonth(2,true),29);
	ASSERT_EQ(DateTime::getLastDayOfMonth(2,false),28);
	ASSERT_EQ(DateTime::getLastDayOfMonth(9,true),30);
	ASSERT_EQ(DateTime::getLastDayOfMonth(9,false),30);
	ASSERT_EQ(DateTime::getLastDayOfMonth(12,true),31);
	ASSERT_EQ(DateTime::getLastDayOfMonth(12,false),31);
}

TEST(Check_skip_date_separators,positive)
{
	ASSERT_STREQ(DateTime::skipDateSeparators(" /-.2016"),"2016");
}

TEST(Check_skip_to_date_separators,positive)
{
	ASSERT_STREQ(DateTime::skipToDateSeparators("Jan 2016")," 2016");
	ASSERT_STREQ(DateTime::skipToDateSeparators("Feb.2016"),".2016");
	ASSERT_STREQ(DateTime::skipToDateSeparators("Mar/2016"),"/2016");
}

TEST(Check_getDateFromStr,positive)
{
	dt_date_t expected = {.year=2016, .month=04, .day = 16 };
	dt_date_t result;

	result.year = 9999;
	ASSERT_TRUE(DateTime::getDateFromStr("2016-Apr-16",&result));
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
	result.year = 9999;
	ASSERT_TRUE(DateTime::getDateFromStr("16.Apr.2016",&result));
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
	result.year = 9999;
	ASSERT_TRUE(DateTime::getDateFromStr("APR 16, 2016",&result));
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
	result.year = 9999;
	ASSERT_TRUE(DateTime::getDateFromStr("16/apr/2016",&result));
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
}


TEST(Check_isDateMoreRecent,positive)
{
	dt_date_t date1 = {.year=1984, .month=07, .day = 26 };
	dt_date_t date2 = {.year=1976, .month=04, .day = 16 };

	ASSERT_TRUE(DateTime::isDateMoreRecent(&date2,&date1));

	dt_date_t date3 = {.year=1984, .month=07, .day = 26 };
	dt_date_t date4 = {.year=1984, .month=04, .day = 16 };
	ASSERT_TRUE(DateTime::isDateMoreRecent(&date4,&date3));

	dt_date_t date5 = {.year=1984, .month=07, .day = 26 };
	dt_date_t date6 = {.year=1984, .month=04, .day = 16 };
	ASSERT_TRUE(DateTime::isDateMoreRecent(&date6,&date5));
}

#if 0
TEST(Check_getDatesDelta1,positive)
{
	dt_date_t date1 = {.year=2016, .month=03, .day = 10 };
	dt_date_t date2 = {.year=2016, .month=02, .day = 20 };
	dt_date_t expected = {.year=0, .month=0, .day = 19 };
	dt_date_t result;

	ASSERT_TRUE(DateTime::getDatesDelta(&date2,&date1,&result));
	printf("year: %d, month: %d, day: %d\n",result.year, result.month, result.day);
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
}

TEST(Check_getDatesDelta2,positive)
{
	dt_date_t date1 = {.year=1984, .month=03, .day = 13 };
	dt_date_t date2 = {.year=1976, .month=04, .day = 16 };
	dt_date_t expected = {.year=7, .month=10, .day = 26 };
	dt_date_t result;

	ASSERT_TRUE(DateTime::getDatesDelta(&date2,&date1,&result));
	printf("year: %d, month: %d, day: %d\n",result.year, result.month, result.day);
	ASSERT_TRUE(DateTime::areDatesEqual(&expected,&result));
}
#endif

TEST(Check_getEpochFromDateTime,positive)
{
	ASSERT_EQ(DateTime::getEpochFromDateTime(2016,2,29,0,0,0),1456704000UL);
	ASSERT_EQ(DateTime::getEpochFromDateTime(2016,3,29,0,0,0),1459209600UL);
	ASSERT_EQ(DateTime::getEpochFromDateTime(1984,3,13,11,11,11),448024271UL);
	ASSERT_EQ(DateTime::getEpochFromDateTime(1976,4,16,23,59,59),198547199UL);
	ASSERT_EQ(DateTime::getEpochFromDateTime(2038,1,19, 3,14, 7),2147483647UL);
}

TEST(Check_setDateTimeFromEpoch,positive)
{
    dt_date_t date;
    dt_time_t time;

    DateTime::setDateTimeFromEpoch(1456704000UL, &date, &time);
    ASSERT_EQ( 2016 , date.year );
    ASSERT_EQ(    2 , date.month );
    ASSERT_EQ(   29 , date.day );
    ASSERT_EQ(    0 , time.hour );
    ASSERT_EQ(    0 , time.minute );
    ASSERT_EQ(    0 , time.second );

    DateTime::setDateTimeFromEpoch(448024271UL, &date, &time);
    ASSERT_EQ( 1984 , date.year );
    ASSERT_EQ(    3 , date.month );
    ASSERT_EQ(   13 , date.day );
    ASSERT_EQ(   11 , time.hour );
    ASSERT_EQ(   11 , time.minute );
    ASSERT_EQ(   11 , time.second );

    DateTime::setDateTimeFromEpoch(2147483647UL, &date, &time);
    ASSERT_EQ( 2038 , date.year );
    ASSERT_EQ(    1 , date.month );
    ASSERT_EQ(   19 , date.day );
    ASSERT_EQ(    3 , time.hour );
    ASSERT_EQ(   14 , time.minute );
    ASSERT_EQ(    7 , time.second );
}
TEST(Check_isHoliday,positive)
{
	ASSERT_TRUE(DateTime::isHoliday(2030,12,26));
	ASSERT_TRUE(DateTime::isHoliday(2017, 1, 1));
	ASSERT_TRUE(DateTime::isHoliday(2017, 1, 6));
	ASSERT_TRUE(DateTime::isHoliday(2030,12,25));
	ASSERT_TRUE(DateTime::isHoliday(2023,11,11));
}

TEST(Check_isHoliday,negative)
{
	ASSERT_FALSE(DateTime::isHoliday(2030,12,27));
	ASSERT_FALSE(DateTime::isHoliday(2016, 1, 1));
	ASSERT_FALSE(DateTime::isHoliday(2017, 1, 7));
	ASSERT_FALSE(DateTime::isHoliday(2023,11, 2));
	ASSERT_FALSE(DateTime::isHoliday(2023,11, 9));
}


TEST(Check_getTimeFromStr,positive)
{
	dt_time_t result;

	dt_time_t expected = {.hour = 0, .minute = 0, .second = 0 };
	ASSERT_TRUE(DateTime::getTimeFromStr("00:00:00",&result));
	ASSERT_TRUE(DateTime::areTimesEqual(&expected,&result));

	expected = {.hour = 23, .minute = 59, .second = 59 };
	ASSERT_TRUE(DateTime::getTimeFromStr(" 23 : 59 : 59",&result));
	ASSERT_TRUE(DateTime::areTimesEqual(&expected,&result));

	expected = {.hour = 1, .minute = 10, .second = 11 };
	ASSERT_TRUE(DateTime::getTimeFromStr(" 0001::10   11:",&result));
	ASSERT_TRUE(DateTime::areTimesEqual(&expected,&result));
}

TEST(Check_getTimeFromStr,negative)
{
	dt_time_t result;
	ASSERT_FALSE(DateTime::getTimeFromStr("00:00",&result));
	ASSERT_FALSE(DateTime::getTimeFromStr("24:00:00",&result));
	ASSERT_FALSE(DateTime::getTimeFromStr("00:60:00",&result));
	ASSERT_FALSE(DateTime::getTimeFromStr("00:00:60",&result));
	ASSERT_FALSE(DateTime::getTimeFromStr("256:00:00",&result));
}

TEST(Check_getWeekDayFromEpoch,positive)
{
	ASSERT_EQ(DateTime::getWeekDayFromEpoch(0UL), DateTime::THURSDAY);
	ASSERT_EQ(DateTime::getWeekDayFromEpoch(448024271UL), DateTime::TUESDAY);
	ASSERT_EQ(DateTime::getWeekDayFromEpoch(198547199UL), DateTime::FRIDAY);
	ASSERT_EQ(DateTime::getWeekDayFromEpoch(4294967294UL), DateTime::SUNDAY);
}

TEST(Check_getDstEpoch,positive)
{
    ASSERT_EQ(DateTime::getDstEpoch(2012, 3, -1, 2), 1332640800UL);
    ASSERT_EQ(DateTime::getDstEpoch(2013, 3, -1, 2), 1364695200UL);
    ASSERT_EQ(DateTime::getDstEpoch(2014, 3, -1, 2), 1396144800UL);

    ASSERT_EQ(DateTime::getDstEpoch(2012, 10, -1, 3), 1351393200UL);
    ASSERT_EQ(DateTime::getDstEpoch(2013, 10, -1, 3), 1382842800UL);
    ASSERT_EQ(DateTime::getDstEpoch(2014, 10, -1, 3), 1414292400UL);
}
