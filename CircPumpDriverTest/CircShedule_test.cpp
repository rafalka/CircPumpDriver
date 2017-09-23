/*
 * CircShedule_test.cpp
 *
 *  Created on: 6 sty 2017
 *      Author: ark036
 */

#include <stdio.h>
#include <gtest/gtest.h>
#include "DateTime.h"
#include "CircShedule.h"


TEST(Check_getNextOnTime,positive)
{
    const circ_shedule_table_t shedule_table = {
            /* Monday */    { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT(  6,00 , 0 ), CT(  8,35, 0 ) }, { CT( 16,40, 0 ), CT( 22,40, 0 ) },  },
            /* Tuesday */   { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT( 16,40 , 0 ), CT( 22,40, 0 ) }, },
            /* Wednesday */ { { CT(23,55, 0 ), CT(24, 0, 0 ) }, },
            /* Thursay */   { { CT( 3,55, 0 ), CT( 4,55, 0 ) }, { CT( 16,40 , 0 ), CT( 23,59,59 ) }, },
            /* Friday */    { { CT( 0, 0, 0 ), CT( 1, 0, 0 ) }, { CT(  3,55 , 0 ), CT(  4,55, 0 ) }, { CT(  7, 0, 0 ), CT( 23,59,59 ) },  },
            /* Saturday */  { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 23,59,59 ) }, },
            /* Sunday */    { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 23,59,59 ) }, },
            /* Holiday */   { { CT( 0, 0, 0 ), CT( 2, 0, 0 ) }, { CT(  8,00 , 0 ), CT( 23,59,59 ) }, }
    };
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 5, 16,  0, 0)) /* Thu */, DateTime::getEpochFromDateTime(2017, 1, 5, 16, 40, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 5, 17,  0, 0)) /* Thu */, DateTime::getEpochFromDateTime(2017, 1, 5, 17,  0, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 6,  7,  0, 0)) /* Hol */, DateTime::getEpochFromDateTime(2017, 1, 6,  8,  0, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 6, 12,  0, 0)) /* Hol */, DateTime::getEpochFromDateTime(2017, 1, 6, 12,  0, 0) );

   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 9,  1,  0, 0)) /* Mon */, DateTime::getEpochFromDateTime(2017, 1, 9,  3, 55, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 9,  4, 30, 0)) /* Mon */, DateTime::getEpochFromDateTime(2017, 1, 9,  4, 30, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 9,  5,  5, 0)) /* Mon */, DateTime::getEpochFromDateTime(2017, 1, 9,  6,  0, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 9,  6, 30, 0)) /* Mon */, DateTime::getEpochFromDateTime(2017, 1, 9,  6, 30, 0) );
   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 9, 23, 30, 0)) /* Mon */, DateTime::getEpochFromDateTime(2017, 1,10,  3, 55, 0) );

   ASSERT_EQ(CircShedule::getNextOnTime(&shedule_table, DateTime::getEpochFromDateTime(2017, 1, 10, 22, 40, 2)) /* Tue */, DateTime::getEpochFromDateTime(2017, 1,11, 23, 55, 0) );
}

