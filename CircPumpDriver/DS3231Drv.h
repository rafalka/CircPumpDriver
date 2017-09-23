/**
 * DS3231Drv.h - Basic driver for the DS3231 RTC module
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

#ifndef DS3231DRV_H_
#define DS3231DRV_H_

#include <DateTime.h>

typedef enum
{
    DS3231_EVERY_SECOND = 0b00001111,
    DS3231_MATCH_S = 0b00001110,
    DS3231_MATCH_M_S = 0b00001100,
    DS3231_MATCH_H_M_S = 0b00001000,
    DS3231_MATCH_DT_H_M_S = 0b00000000,
    DS3231_MATCH_DY_H_M_S = 0b00010000
} DS3231_alarm1_t;

typedef enum
{
    DS3231_EVERY_MINUTE = 0b00001110,
    DS3231_MATCH_M = 0b00001100,
    DS3231_MATCH_H_M = 0b00001000,
    DS3231_MATCH_DT_H_M = 0b00000000,
    DS3231_MATCH_DY_H_M = 0b00010000
} DS3231_alarm2_t;


class DS3231Drv {
private:

    static uint8_t bcd2dec(uint8_t bcd)
    {
        return ((bcd >> 4) * 10) + (bcd & 0x0F);
    }
    static uint8_t dec2bcd(uint8_t bcd)
    {
        return ((bcd/10)<< 4) | (bcd % 10);
    }
public:


    static bool begin();
    static bool enableOscilatorOnBattery(bool enabled);
    static bool writeRequest(const uint8_t* request, uint8_t request_size);
    static bool writeReg(uint8_t reg, uint8_t value)
    {
        uint8_t req[] = { reg, value };
        return writeRequest(req, sizeof(req) );
    }

    static bool readReg(uint8_t reg, uint8_t* request, uint8_t request_size);
    static bool readReg(uint8_t reg, uint8_t* value)
    {
        return readReg(reg, value, 1);
    }

    static bool writeMaskReg(uint8_t reg, uint8_t value, uint8_t mask);
    static bool regBitSet(uint8_t reg, uint8_t bit_number)
    {
        return writeMaskReg(reg, 1 << bit_number, 1 << bit_number);
    }
    static bool regBitClear(uint8_t reg, uint8_t bit_number)
    {
        return writeMaskReg(reg, 0, 1 << bit_number);
    }


    static bool readStatusReg(uint8_t* out_status);
    static bool writeStatusReg(uint8_t status);

    static bool readControlReg(uint8_t* out_control);
    static bool writeControlReg(uint8_t control);


    static bool getDateTime(dt_date_t* date, dt_time_t* time);
private:
    static bool setDateTime(const dt_date_t* date, const dt_time_t* time, uint32_t epoch);
    static bool readAlarm(uint8_t status_reg_bit_maks, bool& result, bool clear);
public:
    static bool setDateTime(uint32_t epoch);
    static bool setDateTime(const dt_date_t* date, const dt_time_t* time);
    static bool clearAlarm1();
    static bool readAlarm1(bool& result, bool clear=true );
    static bool isAlarm1(bool clear=true);
    static bool armAlarm1(bool armed);
    static bool isArmed1();

    static bool clearAlarm2();
    static bool readAlarm2(bool& result, bool clear=true );
    static bool isAlarm2(bool clear = true);
    static bool armAlarm2(bool armed);
    static bool isArmed2();
    static bool setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, DS3231_alarm1_t mode, bool armed = true);
};

#endif /* DS3231DRV_H_ */
