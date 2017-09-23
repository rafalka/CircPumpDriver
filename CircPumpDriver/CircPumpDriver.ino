/**
 * CircPumpDriver.ino - Driver for Water Circulation Pump
 * Based on TI Launchpad MSP-EXP430G2553 with Energia
 * Created and tested on TI Code Composer Studio 6.1.3
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

#ifndef SIMULATION
//#define _DEBUG
#include "DateTime.h" /* Due to some *hacks* this file must be included as first */
#include "CircShedule.h"
#endif


#include <DS3231Drv.h>

#define PRINT(_s_)                    Serial.print(_s_)
#define PRINTLN(_s_)                  Serial.println(_s_)
#include <dbgprint.h>


// ========================================================================================================= Consts

const uint8_t PUMP_PIN = P2_3;
const uint8_t MODE_BTN_PIN = PUSH2; //Pin 1_3

const unsigned TICK_TIME = 100; //ms
const unsigned HEARTBEAT_ON_TICKS = 10;
const unsigned HEARTBEAT_OFF_TICKS = 2;
const unsigned RTC_READ_TICKS=5;
const unsigned CHECK_PUMP_TICKS=5;
const unsigned MODE_CHANGE_TICKS=20; // 2s
#if 1
static const uint16_t ON_TIME_FIRST  =  4 * 60;
static const uint16_t OFF_TIME_FIRST =  6 * 60;
static const uint16_t ON_TIME_NEXT   =  1 * 60;
static const uint16_t OFF_TIME_NEXT  =  9 * 60;
#else
// Used for testing
static const uint16_t ON_TIME_FIRST  =  2;
static const uint16_t OFF_TIME_FIRST =  3;
static const uint16_t ON_TIME_NEXT   =  2;
static const uint16_t OFF_TIME_NEXT  =  3;
#endif

// ========================================================================================================= Shedule Tables
#if 0
static const circ_shedule_table_t workweek_shedule_table = {
        /* Monday */    { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT(  8,35, 0 ) }, { CT( 16,40, 0 ), CT( 22,40, 0 ) },  },
        /* Tuesday */   { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT( 16,40 , 0 ), CT( 22,40, 0 ) }, },
        /* Wednesday */ { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT( 16,40 , 0 ), CT( 22,40, 0 ) }, },
        /* Thursay */   { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT( 16,40 , 0 ), CT( 24, 0, 0 ) }, },
        /* Friday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  3,55 , 0 ), CT(  4,55, 0 ) }, { CT(  7, 0, 0 ), CT( 24, 0, 0 ) },  },
        /* Saturday */  { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Sunday */    { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Holiday */   { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, }
};
#else
static const circ_shedule_table_t workweek_shedule_table = {
        /* Monday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Tuesday */   { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Wednesday */ { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Thursay */   { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Friday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Saturday */  { { CT( 0, 0, 0 ), CT( 3, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Sunday */    { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Holiday */   { { CT( 0, 0, 0 ), CT( 3, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, }
};
#endif
static const circ_shedule_table_t vacations_shedule_table = {
        /* Monday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Tuesday */   { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Wednesday */ { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Thursay */   { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Friday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Saturday */  { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Sunday */    { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, },
        /* Holiday */   { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 24, 0, 0 ) }, }
};

static const circ_shedule_table_t* current_shedule_table = &workweek_shedule_table;

static inline bool isWorkweekScheduleTable()   { return current_shedule_table == &workweek_shedule_table; }
static inline void setWorkweekScheduleTable()  { current_shedule_table = &workweek_shedule_table; }
static inline void setVacationsScheduleTable() { current_shedule_table = &vacations_shedule_table; }


// ========================================================================================================= Globals

uint32_t current_rtc_time;
uint32_t current_local_time;

uint32_t ticks=0;        ///< System ticks. Increased every TICK_TIME ms

uint32_t last_pump_off_time=0;
static bool pump_on = false;

void DisplayDateTime(const char* start_str, uint32_t epoch);
// ========================================================================================================= System utilities
static inline void reset() {
    WDTCTL = 0; // Writing to Watchdog control register without "password" ( 05Ah in the upper byte) causes reset
}

#ifdef _DEBUG

static void dead_loop()
{
    for (int i=0;;i++) {
        digitalWrite(RED_LED, i&1 ? HIGH : LOW );
        digitalWrite(GREEN_LED, i&1 ? LOW : HIGH );
        delay(200);
    }

}

#define ASSERT(_expr_) do { auto i = _expr_; \
    if (! i) { \
        DisplayDateTime("ASSERT: ", current_local_time); \
        PRINT5(#_expr_, " : ",__FILE__,":",__LINE__);\
        dead_loop(); \
    } } while(0)
#else
# define ASSERT(_expr_) do { auto i = _expr_; if (! i) reset(); } while(0)
#endif
// ========================================================================================================= RTC Handling
// Repeat operation twice and do reset if failing
#define RTC_CHECK( _operation_ ) do { \
    if (! (_operation_) ) { \
        ASSERT(_operation_); \
    } } while(0)


static void initRtc() {
    DS3231Drv::begin();
    RTC_CHECK(DS3231Drv::enableOscilatorOnBattery(true));
    RTC_CHECK(DS3231Drv::armAlarm1(false));
    RTC_CHECK(DS3231Drv::armAlarm2(false));
    RTC_CHECK(DS3231Drv::clearAlarm1());
    RTC_CHECK(DS3231Drv::clearAlarm2());
}

static uint32_t getBuildDateTime() {
    dt_date_t build_date;
    if (! DateTime::getDateFromStr(__DATE__, &build_date) ) return DateTime::EPOCH_ERROR;
    dt_time_t build_time;
    if (! DateTime::getTimeFromStr(__TIME__, &build_time) ) return DateTime::EPOCH_ERROR;

    return DateTime::getEpochFromDateTime(&build_date,&build_time);
}

static uint32_t getRtcDateTime() {
    dt_date_t d;
    dt_time_t t;
    RTC_CHECK( DS3231Drv::getDateTime(&d,&t) );
    return DateTime::getEpochFromDateTime(&d,&t);
}

static void setRtcDateTime(uint32_t datetime_to_set)
{
    if (datetime_to_set== DateTime::EPOCH_ERROR) return;
    RTC_CHECK( DS3231Drv::setDateTime(datetime_to_set) );
    uint32_t datetime_to_verify = getRtcDateTime();
    ASSERT( (datetime_to_verify-datetime_to_set)<10 );
}


static void readDateTimeFromRtc()
{
    current_rtc_time = getRtcDateTime();
    current_local_time = DateTime::getLocalDateTimeFromUtc( current_rtc_time );
}



static void DisplayDateTime( const char* start_str, uint32_t epoch)
{
    if(start_str) {
        PRINT(start_str);
    }
    if (epoch==DateTime::EPOCH_ERROR)
    {
        PRINTLN("Epoch ERROR!!!\n");
        return;
    }

    dt_date_t d;
    dt_time_t t;
    DateTime::setDateTimeFromEpoch(epoch, &d, &t);
    PRINT4(d.year,"-", DateTime::getMonthAbbrev(static_cast<DateTime::MONTHS>(d.month)),"-");
    PRINT4(d.day,", ",DateTime::getDayAbbrev(DateTime::getDayTypeFromEpoch(epoch)),", ");
    PRINTLN5(t.hour,":",t.minute,":",t.second);
}

static void ReadAndAdjustRTC()
{
    auto build = getBuildDateTime();
    DisplayDateTime("Build date:  ", build );
    // Build date is in local time
    build = DateTime::getUtcDateTimeFromLocal( build );
    auto rtc = getRtcDateTime();
    DisplayDateTime("RTC date:    ", DateTime::getLocalDateTimeFromUtc(rtc) );
    if ( build==DateTime::EPOCH_ERROR ) return;

    if ( (rtc<build && (build-rtc)>30) || rtc==DateTime::EPOCH_ERROR ) {
        PRINTLN("Adjusting RTC to current build date...");

        setRtcDateTime(build);
    }
}

// ========================================================================================================= Circulation Pump handling

static inline bool isCircOnOffTime() {
    bool isAlarm = false;
    RTC_CHECK( DS3231Drv::readAlarm1(isAlarm) );
    return isAlarm;
}

static void setCircOnOffRtcTime(uint32_t epoch)
{
    if (epoch== DateTime::EPOCH_ERROR) return;
    dt_date_t d;
    dt_time_t t;

    DateTime::setDateTimeFromEpoch( epoch, &d, &t );
    RTC_CHECK( DS3231Drv::setAlarm1(d.day, t.hour, t.minute, t.second, DS3231_MATCH_DT_H_M_S) );
}



static inline bool isCircPumpOn() { return pump_on; }
static void setCircPumpOnOff(bool on)
{
   DisplayDateTime(on ? "Pump is ON:  " : "Pump is OFF: ", current_local_time);
   pump_on = on;
   digitalWrite(PUMP_PIN, on ? HIGH : LOW);
   if (!on) last_pump_off_time = current_rtc_time;
}

static inline uint32_t getNextOffRtcTime() {
    // First time after longer perdiod we'll let Pump be on for longer time as pipes may be colder
    return current_rtc_time + ( ( (current_rtc_time-last_pump_off_time) > 2*OFF_TIME_FIRST ) ? ON_TIME_FIRST : ON_TIME_NEXT );
}
static inline void setupCircOnOffRtcTime(uint32_t time) {
    setCircOnOffRtcTime(time);
    IFDEBUG( DisplayDateTime("Next event:  ",DateTime::getLocalDateTimeFromUtc(time) ) );
}

static inline uint32_t getNextOnRtcTime() {
    uint32_t on_time =  current_rtc_time + ( ( (current_rtc_time-last_pump_off_time) > 2*OFF_TIME_FIRST ) ? OFF_TIME_FIRST : OFF_TIME_NEXT );
    // Schedule tables are in local time
    on_time = DateTime::getUtcDateTimeFromLocal(
                CircShedule::getNextOnTime(current_shedule_table, DateTime::getLocalDateTimeFromUtc( on_time ) )
            );
    // Check Daylight Saving Time case
    if (on_time < current_rtc_time) {
        on_time += ((current_rtc_time - on_time) / DateTime::ONE_HOUR + 1) * DateTime::ONE_HOUR;
    }
    return on_time ;
}

static void setupFirstOn() {
    uint32_t time = CircShedule::getNextOnTime(current_shedule_table, current_local_time);
    if (time == current_local_time) {
        time = getNextOffRtcTime();
        setCircPumpOnOff(true);
    } else {
        time = DateTime::getUtcDateTimeFromLocal( time );
        setCircPumpOnOff(false);
    }
    setupCircOnOffRtcTime(time);
}


static void CheckCircPumpEvent() {
    if (isCircOnOffTime()) {
        bool on = isCircPumpOn();
        uint32_t t = (on) ? getNextOnRtcTime() : getNextOffRtcTime();
        setCircPumpOnOff(!on);
        setupCircOnOffRtcTime(t);
    }
}

// ========================================================================================================= Heartbeat handling

static inline void setHeartbeatLedOnOff(bool on) {
    digitalWrite(GREEN_LED, on ? HIGH : LOW );
}
static inline void setModeLedOnOff(bool on) {
    digitalWrite(RED_LED, on ? HIGH : LOW );
}

// ========================================================================================================= Mode Button handling

static void ChangePumpScheduleMode()
{
    bool workweek_mode = isWorkweekScheduleTable();

    if (workweek_mode) {
        PRINTLN("Switching to vacations mode...");
        setVacationsScheduleTable();
    } else {
        PRINTLN("Switching to work week mode...");
        setWorkweekScheduleTable();
    }

    setModeLedOnOff( workweek_mode );
    setupFirstOn();
}

static bool isModeButtonOn()
{
    return (digitalRead(MODE_BTN_PIN)==HIGH) ? false : true;
}

static void HandleModeButton()
{
    static const uint16_t ALREADY_HANDLED= 0xFFFF;
    static uint16_t on_timer = 0;

    if (! isModeButtonOn()) {
        on_timer = 0;
        return;
    } else if ( on_timer==ALREADY_HANDLED ) {
        return;
    }

    on_timer++;

    if (on_timer>MODE_CHANGE_TICKS) {
        on_timer = ALREADY_HANDLED;
        ChangePumpScheduleMode();
    }
}

// ========================================================================================================= setup()
void setup()
{
  Serial.begin(9600);

  // Initialize DS3231
  PRINTLN("Initializing Circulation Pump Driver...");

  digitalWrite(PUMP_PIN, LOW);
  pinMode(PUMP_PIN, OUTPUT);

  pinMode(MODE_BTN_PIN, INPUT_PULLUP);

  digitalWrite(RED_LED, LOW);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);


  initRtc();
  ReadAndAdjustRTC();

  readDateTimeFromRtc();
  setupFirstOn();
  PRINTLN("Entering main loop...");
}

// ========================================================================================================= loop()

void loop()
{
    ticks++;

    if (!(ticks % RTC_READ_TICKS)) {
        readDateTimeFromRtc();
    }

    HandleModeButton();

    if ( !(ticks % CHECK_PUMP_TICKS) ) {
        CheckCircPumpEvent();
    }

    if (!(ticks % HEARTBEAT_ON_TICKS) ) {
        setHeartbeatLedOnOff(true);
    } else  if (!(ticks % HEARTBEAT_OFF_TICKS) ) {
        setHeartbeatLedOnOff(false);
    }

    delay(TICK_TIME);
}

