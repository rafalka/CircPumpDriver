/**
 * CircShedule.cpp - On/Off table handling class
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

#include <CircShedule.h>


uint32_t CircShedule::getNextOnTime(const circ_shedule_table_t* shedule_table, uint32_t epoch)
{
    if (!shedule_table || epoch == DateTime::EPOCH_ERROR) return DateTime::EPOCH_ERROR;

    dt_time_t time;
    DateTime::setDateTimeFromEpoch(epoch, nullptr, &time);
    uint16_t ct = CT(time.hour, time.minute, time.second);
    uint8_t day = DateTime::getDayTypeFromEpoch(epoch);

    const circ_shedule_entry_t* day_table = &((*shedule_table)[day][0]);

    for (int i=0; i<CIRC_PERIODS_PER_DAY && day_table->beg< day_table->end; i++, day_table++)
    {
        if (ct<day_table->beg)
        {
            return epoch + CT_TO_S(day_table->beg - ct);
        }
        else if (ct>= day_table->beg && ct <= day_table->end)
        {
            return epoch;
        }
    }
    // We found not entry for this day, so get first entry for next day
    epoch += DateTime::ONE_DAY;
    day = DateTime::getDayTypeFromEpoch(epoch);
    return epoch - CT_TO_S(ct- ((*shedule_table)[day][0]).beg);
}
