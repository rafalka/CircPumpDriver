/**
 * CircShedule.h - On/Off table handling class
 * This file is a part of Water Circulation Pump driver
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

#ifndef CIRCSHEDULE_H_
#define CIRCSHEDULE_H_

#include "DateTime.h"
#define CIRC_PERIODS_PER_DAY 3

#define S_TO_CT(_seconds_) ((_seconds_)>>1)
#define CT_TO_S(_dt_)      ((uint32_t)(_dt_)<<1)
#define CT(_hour_,_minute_,_second_) ((uint16_t) S_TO_CT(3600UL*(_hour_)+60UL*(_minute_)+(_second_)))
typedef struct circ_shedule_entry_s 
{
    uint16_t beg;
    uint16_t end;
} circ_shedule_entry_t;

typedef circ_shedule_entry_t circ_shedule_day_t[CIRC_PERIODS_PER_DAY];

typedef circ_shedule_entry_t circ_shedule_table_t[DateTime::DAYS_COUNT][CIRC_PERIODS_PER_DAY];


class CircShedule {
public:
    static uint32_t getNextOnTime(const circ_shedule_table_t* shedule_table, uint32_t timestamp);
};

#endif /* CIRCSHEDULE_H_ */
