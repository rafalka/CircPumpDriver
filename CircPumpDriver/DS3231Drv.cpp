/**
 * DS3231Drv.cpp - Basic driver for the DS3231 RTC module
 * Inspired  by DS3231 driver 2014 Korneliusz Jarzebski
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


#include <DS3231Drv.h>

#if 0
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#endif 

extern "C" {
#include <twi.h>
}


#define DS3231_ADDRESS              (0x68)

#define DS3231_REG_TIME             (0x00)
#define DS3231_REG_ALARM_1          (0x07)
#define DS3231_REG_ALARM_2          (0x0B)
#define DS3231_REG_CONTROL          (0x0E)
#define DS3231_REG_STATUS           (0x0F)
#define DS3231_REG_TEMPERATURE      (0x11)

/*
Control Register(0Eh) Bits:

Bit 7: Enable Oscillator(EOSC).
    When set to logic 0, the oscillator is started.When set to logic 1, the oscillator
    is stopped when the DS3231 switches to VBAT.This bit is clear(logic 0) when power is first applied. 
    When the DS3231 is powered by VCC, the oscillator is always on regardless of the status of the EOSC bit.
    When EOSC is disabled, all register data is static. */
#define  DS3231_REG_CONTROL_EOSC_BIT          7
#define  DS3231_REG_CONTROL_EOSC_BIT_MASK     (1<<DS3231_REG_CONTROL_EOSC_BIT)

/*
Bit 6: Battery - Backed Square - Wave Enable (BBSQW).
    When set to logic 1 with INTCN = 0 and VCC < VPF, this bit enables the square wave. When BBSQW is logic 0,
    the INT / SQW pin goes high impedance when VCC < VPF. This bit is disabled(logic 0) when power is first applied.
*/
#define  DS3231_REG_CONTROL_BBSQW_BIT         6
#define  DS3231_REG_CONTROL_BBSQW_BIT_MASK    (1<<DS3231_REG_CONTROL_BBSQW_BIT)

/*
Bit 5: Convert Temperature (CONV).
    Setting this bit to 1 forces the temperature sensor to convert the temperature into digital code 
    and execute the TCXO algorithm to update the capacitance array to the oscillator.
*/
#define  DS3231_REG_CONTROL_CONV_BIT          5
#define  DS3231_REG_CONTROL_CONV_BIT_MASK     (1<<DS3231_REG_CONTROL_CONV_BIT)

/*
Bits 4 and 3: Rate Select (RS2 and RS1). 
    These bits control the frequency of the square-wave output when the square wave has been enabled. 
    The following table shows the square-wave frequencies that can be selected with the RS bits.  
    These bits are both set to logic 1 (8.192kHz) when power is first applied.
    RS2 RS1    SQUARE-WAVE OUTPUT FREQUENCY
    0   0      1Hz
    0   1      1.024kHz
    1   0      4.096kHz
    1   1      8.192kHz
*/
#define  DS3231_REG_CONTROL_RS2_BIT           4
#define  DS3231_REG_CONTROL_RS1_BIT           3
#define  DS3231_REG_CONTROL_RS2_BIT_MASK      (1<<DS3231_REG_CONTROL_RS2_BIT)
#define  DS3231_REG_CONTROL_RS1_BIT_MASK      (1<<DS3231_REG_CONTROL_RS1_BIT)

/*
Bit 2: Interrupt Control (INTCN). 
    This bit controls the INT/SQW signal. When the INTCN bit is set to logic 0,
    a square wave is output on the INT/SQW pin. When the INTCN bit is set to logic 1, then a match between 
    the time keeping registers and either of the alarm registers activates the INT/SQW output (if the alarm is also enabled).
    The corresponding alarm flag is always set regardless of the state of the INTCN bit. 
    The INTCN bit is set to logic 1 when power is first applied.
*/
#define  DS3231_REG_CONTROL_INTCN_BIT         2
#define  DS3231_REG_CONTROL_INTCN_BIT_MASK    (1<<DS3231_REG_CONTROL_INTCN_BIT)

/*
Bit 1: Alarm 2 Interrupt Enable (A2IE).  
    When set to logic 1, this bit permits the alarm 2 flag (A2F) bit in the status register to assert INT/SQW (when INTCN = 1).
    When the A2IE bit is set to logic 0 or INTCN is set to logic 0, the A2F bit does not initiate an interrupt signal.  
    The A2IE bit is disabled (logic 0) when power is first applied.
*/
#define  DS3231_REG_CONTROL_A2IE_BIT          1
#define  DS3231_REG_CONTROL_A2IE_BIT_MASK     (1<<DS3231_REG_CONTROL_A2IE_BIT)

/*
Bit 0: Alarm 1 Interrupt Enable (A1IE).  
    When set to logic 1, this bit permits the alarm 1 flag (A1F) bit in the status register to assert INT/SQW (when INTCN = 1).
    When the A1IE bit is set to logic 0 or INTCN is set to logic 0, the A1F bit does not initiate the INT/SQW signal.
    The A1IE bit is disabled (logic 0) when power is first applied.
*/
#define  DS3231_REG_CONTROL_A1IE_BIT          0
#define  DS3231_REG_CONTROL_A1IE_BIT_MASK     (1<<DS3231_REG_CONTROL_A1IE_BIT)


/*
Status Register (0Fh) Bits:

Bit 7: Oscillator Stop Flag (OSF). 
    A logic 1 in this bit indicates that the oscillator either is stopped or was stopped for some period and may be used to judge the validity of
    the timekeeping data. This bit is set to logic 1 any time that the oscillator stops. The following are examples of
    conditions that can cause the OSF bit to be set:
        1)  The first time power is applied.
        2)  The voltages present on both VCC and VBAT are insufficient to support oscillation.
        3)  The EOSC bit is turned off in battery-backed mode.
        4)  External influences on the crystal (i.e., noise, leakage,etc.).
    This bit remains at logic 1 until written to logic 0.
*/
#define  DS3231_REG_STATUS_OSF_BIT            7
#define  DS3231_REG_STATUS_OSF_BIT_MASK       (1<<DS3231_REG_STATUS_OSF_BIT)


/*
Bit 3: Enable 32kHz Output (EN32kHz).
    This bit controls the status of the 32kHz pin. When set to logic 1, the 32kHz pin is enabled and outputs a 32.768kHz square wave signal.
    When set to logic 0, the 32kHz pin goes to a high-impedance state. The initial power-up state of this bit is logic 1, and a 32.768kHz square-wave signal 
    appears at the 32kHz pin after a power source is applied to the DS3231 (if the oscillator is running).
*/
#define  DS3231_REG_STATUS_EN32KHZ_BIT        3
#define  DS3231_REG_STATUS_EN32KHZ_BIT_MASK   (1<<DS3231_REG_STATUS_EN32KHZ_BIT)

/*
Bit 2: Busy (BSY).  
    This bit indicates the device is busy executing TCXO functions. It goes to logic 1 when the con version signal to the temperature sensor is asserted 
    and then is cleared when the device is in the 1-minute idle state.
*/
#define  DS3231_REG_STATUS_BSY_BIT            2
#define  DS3231_REG_STATUS_BSY_BIT_MASK       (1<<DS3231_REG_STATUS_BSY_BIT)

/*
Bit 1: Alarm 2 Flag (A2F). 
    A logic 1 in the alarm 2 flag bit indicates that the time matched the alarm 2 registers. 
    If the A2IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted. A2F is cleared when written to logic 0. 
    This bit can only be written to logic 0. Attempting to write to logic 1 leaves the value unchanged.
*/
#define  DS3231_REG_STATUS_A2F_BIT            1
#define  DS3231_REG_STATUS_A2F_BIT_MASK       (1<<DS3231_REG_STATUS_A2F_BIT)

/*
Bit 0: Alarm 1 Flag (A1F).
    A logic 1 in the alarm 1 flag bit indicates that the time matched the alarm 1 registers.
    If the A1IE bit is logic 1 and the INTCN bit is set to logic 1, the INT/SQW pin is also asserted. A1F is cleared when written to logic 0.
    This bit can only be written to logic 0. Attempting to write to logic 1 leaves the value unchanged.
 */
#define  DS3231_REG_STATUS_A1F_BIT            0
#define  DS3231_REG_STATUS_A1F_BIT_MASK       (1<<DS3231_REG_STATUS_A1F_BIT)



// Requests definition
#pragma pack(push,1)


template<typename ReqType> struct ds3231_req
{
    static const uint8_t size = sizeof(ReqType);
    union {
        uint8_t buf[size];
        ReqType req;
    };
};

template<typename ReqType> struct ds3231_reg_req
{
    static const uint8_t size = sizeof(ReqType) + 1;
    union {
        uint8_t buf[size];
        struct {
            uint8_t reg_addr;
            ReqType req;
        };
    };
};

struct ds3231_time_s {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
};
struct ds3231_date_s {
    uint8_t day;
    uint8_t month;
    uint8_t year;
};
struct ds3231_datetime_s {
    struct ds3231_time_s time;
    uint8_t              day_of_week;
    struct ds3231_date_s date;
};
typedef ds3231_req<ds3231_time_s> ds3231_time_req;
static_assert(sizeof(ds3231_time_req) == ds3231_time_req::size, "Wrong compiler alignment configuration");

typedef ds3231_req<ds3231_date_s> ds3231_date_req;
static_assert(sizeof(ds3231_date_req) == ds3231_date_req::size, "Wrong compiler alignment configuration");


typedef ds3231_req<ds3231_datetime_s> ds3231_datetime_req;
static_assert(sizeof(ds3231_datetime_req) == ds3231_datetime_req::size, "Wrong compiler alignment configuration");

typedef ds3231_reg_req<ds3231_datetime_req> ds3231_datetime_reg_req;
static_assert(sizeof(ds3231_datetime_reg_req) == ds3231_datetime_reg_req::size, "Wrong compiler alignment configuration");

struct ds3231_alarm1_s {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
};

typedef ds3231_req<ds3231_alarm1_s> ds3231_alarm1_req;
static_assert(sizeof(ds3231_alarm1_req) == ds3231_alarm1_req::size, "Wrong compiler alignment configuration");

typedef ds3231_reg_req<ds3231_alarm1_req>  ds3231_alarm1_reg_req;
static_assert(sizeof(ds3231_alarm1_reg_req) == ds3231_alarm1_reg_req::size, "Wrong compiler alignment configuration");

#pragma pack(pop)

bool DS3231Drv::begin()
{
    twi_init();

    return true;
}

// First byte of request buffer must be a register address
bool DS3231Drv::writeRequest(const uint8_t* request, uint8_t request_size)
{
    return ( twi_writeTo(DS3231_ADDRESS, const_cast<uint8_t*>(request), request_size, true, true) == 0 );
}

bool DS3231Drv::readReg(uint8_t reg, uint8_t* request, uint8_t request_size)
{
    if ( twi_writeTo(DS3231_ADDRESS, &reg, 1, true, true) ) return false;
    if ( twi_readFrom( DS3231_ADDRESS, request, request_size, true) != request_size) return false;

    return true;
}

bool DS3231Drv::writeMaskReg(uint8_t reg, uint8_t value, uint8_t mask)
{
    uint8_t read;
    if (! readReg(reg,&read) ) return false;

    read &= ~mask;
    read |= value & mask;

    return writeReg(reg, read);
}


bool DS3231Drv::readStatusReg(uint8_t* out_status)
{
    return readReg(DS3231_REG_STATUS, out_status, 1);
}

bool DS3231Drv::writeStatusReg(uint8_t status)
{
    return writeReg(DS3231_REG_STATUS, status);
}

bool DS3231Drv::readControlReg(uint8_t* out_control)
{
    return readReg(DS3231_REG_CONTROL, out_control, 1);
}

bool DS3231Drv::writeControlReg(uint8_t control)
{
    return writeReg(DS3231_REG_CONTROL, control);
}


bool DS3231Drv::getDateTime(dt_date_t* date, dt_time_t* time)
{
    bool result = true;

    ds3231_datetime_req req;

    if (!readReg(DS3231_REG_TIME, req.buf,req.size) ) return false;

    if (date) {
        date->year =   bcd2dec(req.req.date.year) + 2000;
        date->month =  bcd2dec(req.req.date.month & 0x7F);
        date->day =    bcd2dec(req.req.date.day);

        result &= DateTime::isDateValid(date);
    }
    if (time) {
        time->hour =   bcd2dec(req.req.time.hour & 0x3F);
        time->minute = bcd2dec(req.req.time.minute);
        time->second = bcd2dec(req.req.time.second);

        result &= DateTime::isTimeValid(time);
    }

    return result;
}



bool DS3231Drv::setDateTime(const dt_date_t* date, const dt_time_t* time, uint32_t epoch)
{
    ds3231_datetime_reg_req req;

    req.req.req.date.year   = dec2bcd(date->year - 2000);
    req.req.req.date.month  = dec2bcd(date->month);
    req.req.req.date.day    = dec2bcd(date->day);
    req.req.req.day_of_week = DateTime::getWeekDayFromEpoch(epoch)+1;
    req.req.req.time.hour   = dec2bcd(time->hour);
    req.req.req.time.minute = dec2bcd(time->minute);
    req.req.req.time.second = dec2bcd(time->second);

    req.reg_addr = DS3231_REG_TIME;
    return writeRequest(req.buf,req.size);
}

bool DS3231Drv::setDateTime(uint32_t epoch)
{
    dt_date_t date;
    dt_time_t time;
    DateTime::setDateTimeFromEpoch(epoch, &date, &time);
    return  setDateTime(&date, &time, epoch);
}
bool DS3231Drv::setDateTime(const dt_date_t* date, const dt_time_t* time)
{
    return setDateTime(date, time, DateTime::getEpochFromDateTime(date, time));
}


bool DS3231Drv::readAlarm(uint8_t status_reg_bit_maks, bool& result,  bool clear)
{
    uint8_t status;
    if ( !readStatusReg(&status) ) return false;

    result = (status & status_reg_bit_maks) ? true : false;

    if (result && clear)
    {
        status ^= status_reg_bit_maks;
        if (! writeStatusReg( status ) ) return false;
    }
    return true;
}

bool DS3231Drv::clearAlarm1(void)
{
    bool ignore = false;
    return readAlarm1(ignore,true);
}

bool DS3231Drv::readAlarm1(bool& result, bool clear)
{
    return readAlarm(DS3231_REG_STATUS_A1F_BIT_MASK, result, clear);
}

bool DS3231Drv::isAlarm1(bool clear)
{
    bool isAlarm = false;
    readAlarm1(isAlarm,clear);
    return isAlarm;
}

bool DS3231Drv::armAlarm1(bool armed)
{
    uint8_t value = armed ? DS3231_REG_CONTROL_A1IE_BIT_MASK : 0;

    return  writeMaskReg(DS3231_REG_CONTROL, value, DS3231_REG_CONTROL_A1IE_BIT_MASK);
}

bool DS3231Drv::isArmed1(void)
{
    uint8_t value;
    if (! readStatusReg(&value) ) return false;
    return (value & DS3231_REG_CONTROL_A1IE_BIT_MASK) ? true : false;
}


bool DS3231Drv::clearAlarm2(void)
{
    bool ignore = false;
    return readAlarm2(ignore,true);
}

bool DS3231Drv::readAlarm2(bool& result, bool clear) {
    return readAlarm(DS3231_REG_STATUS_A2F_BIT_MASK, result, clear);
}

bool DS3231Drv::isAlarm2(bool clear)
{
    bool isAlarm = false;
    readAlarm2(isAlarm,clear);
    return isAlarm;
}

bool DS3231Drv::armAlarm2(bool armed)
{
    uint8_t value = armed ? DS3231_REG_CONTROL_A2IE_BIT_MASK : 0;

    return  writeMaskReg(DS3231_REG_CONTROL, value, DS3231_REG_CONTROL_A2IE_BIT_MASK);
}

bool DS3231Drv::isArmed2(void)
{
    uint8_t value;
    if (!readStatusReg(&value)) return false;
    return (value & DS3231_REG_CONTROL_A2IE_BIT_MASK) ? true : false ;
}


bool DS3231Drv::setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed)
{
    static const uint8_t FIELD_IGNORE_MASK = 0b10000000;
    static const uint8_t FIELD_MATCH_MASK = static_cast<uint8_t>(~FIELD_IGNORE_MASK);
    static const uint8_t FIELD_MATCH_DAY_OF_WEEK_MASK = 0b01000000;

    second = dec2bcd(second) & FIELD_MATCH_MASK;
    minute = dec2bcd(minute) & FIELD_MATCH_MASK;
    hour   = dec2bcd(hour)   & FIELD_MATCH_MASK;
    dydw   = dec2bcd(dydw)   & FIELD_MATCH_MASK;

    switch (mode)
    {
        // NOTE: no break;'s here is intented
    case DS3231_EVERY_SECOND:
        second |= FIELD_IGNORE_MASK;
    case DS3231_MATCH_S:
        minute |= FIELD_IGNORE_MASK;
    case DS3231_MATCH_M_S:
        hour   |= FIELD_IGNORE_MASK;
    case DS3231_MATCH_H_M_S:
        dydw   |= FIELD_IGNORE_MASK;
        break;
    case DS3231_MATCH_DY_H_M_S:
        dydw   |= FIELD_MATCH_DAY_OF_WEEK_MASK;
        break;
    case DS3231_MATCH_DT_H_M_S:
    default:
        break;
    }

    struct ds3231_reg_req<ds3231_alarm1_req> request;
    request.reg_addr       = DS3231_REG_ALARM_1;
    request.req.req.second = second;
    request.req.req.minute = minute;
    request.req.req.hour   = hour;
    request.req.req.day    = dydw;

    if (!  writeRequest(request.buf, request.size) ) return false;

    if (! armAlarm1(armed) ) return false;
    clearAlarm1();

    return true;
}

bool DS3231Drv::enableOscilatorOnBattery(bool enabled) {
    uint8_t control;
    if (!readControlReg(&control)) return false;

    if (enabled)
    {
        control &= 0b01111011;
    } else {
        control |= 0b10000000;
    }


    return writeControlReg(control);
}
