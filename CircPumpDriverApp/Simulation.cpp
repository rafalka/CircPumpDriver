#include "DateTime.h" /* Due to some *hacks* this file must be included first */
#include "CircShedule.h"

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>


#define SIMULATION

// Stubs 
#define HIGH 1
#define LOW 0
#define OUTPUT 0

#define GREEN_LED 0
#define RED_LED 1

#define P2_3 2
#define PUSH2 3

static const char* pins_names[] = {
    "GREEN LED",
    "RED LED",
    "PUMP RELAY",
    "MODE BUTTON"
};

#define pinMode(PUMP_PIN, OUTPUT)


void digitalWrite(uint8_t pin, uint8_t val)
{
    if (!pin) return;
    const char* pin_name = (pin < sizeof(pins_names) / sizeof(*pins_names)) ? pins_names[pin] : "unknown";
    std::cout << "Pin: " << pin_name << " set to " << (val ? "HIGH" : "LOW") << std::endl;
}
int digitalRead(uint8_t) { return LOW; }


class
{
public:
    void begin(int) {}
    int print(int i) { return printf("%d",i); }
    int print(const char* s) { return printf("%s",s); }
    int println(int i) { return printf("%d\n", i); }
    int println(const char* s) { return printf("%s\n", s); }

} Serial;

uint32_t rtc_time = 0;

dt_date_t alarm_date;
dt_time_t alarm_time;

struct RTCDateTime
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;
    uint32_t unixtime;
};

static RTCDateTime dt;
class DS3231
{
public:
    void begin() {}
    void clearAlarm1() {}
    void clearAlarm2() {}
    void armAlarm1(bool) {}
    void armAlarm2(bool) {}


    void setDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
    {
        rtc_time = DateTime::getEpochFromDateTime(year, month, day, hour, minute, second);
    }
    bool isAlarm1()
    {
        dt_date_t d;
        dt_time_t t;

        DateTime::setDateTimeFromEpoch(rtc_time, &d, &t);

        return (alarm_date.day == d.day ) && 
               ( alarm_time.hour == t.hour ) &&
               ( alarm_time.minute == t.minute ) &&
               ( alarm_time.second == t.second );
    }
    void setAlarm1(uint8_t dydw, uint8_t hour, uint8_t minute, uint8_t second, int mode, bool armed=true)
    {
        alarm_date.day = dydw;
        alarm_time.hour = hour;
        alarm_time.minute = minute;
        alarm_time.second = second;
    }

    struct RTCDateTime getDateTime()
    {
        dt_date_t d;
        dt_time_t t;

        DateTime::setDateTimeFromEpoch(rtc_time, &d, &t);
        dt.year = d.year;
        dt.month = d.month;
        dt.day = d.day;
        dt.hour = t.hour;
        dt.minute = t.minute;
        dt.second = t.second;

        return dt;
    }
    
};

void delay(int) { }

/*
#define PRINT(_s_)                    do { std::cout << _s_; } while(0)
#define PRINTLN(_s_)                  do { std::cout << _s_ << std::endl; } while(0)
#include <dbgprint.h>
*/

// ========================================================================================================= TwoWire class simlation
#include <Wire.h>
TwoWire Wire;

size_t TwoWire::write(unsigned long n)
{
    return 1;
}
uint8_t TwoWire::requestFrom(uint8_t, uint8_t)
{
    return 1;
}

int TwoWire::read()
{
    return 1;
}

int TwoWire::available()
{
    return 1;
}

extern "C" {
#include <twi.h>
}
/*
* Function twi_writeTo
* Desc     attempts to become twi bus master and write a
*          series of bytes to a device on the bus
* Input    address: 7bit i2c device address
*          data: pointer to byte array
*          length: number of bytes in array
*          wait: boolean indicating to wait for write or not
* Output   0 .. success
*          1 .. length to long for buffer
*          2 .. address send, NACK received
*          3 .. data send, NACK received
*          4 .. other twi error (lost bus arbitration, bus error, ..)
*/
uint8_t twi_writeTo(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait, uint8_t sendStop)
{
    return 0;
}

/*
* Function twi_readFrom
* Desc     attempts to become twi bus master and read a
*          series of bytes from a device on the bus
* Input    address: 7bit i2c device address
*          data: pointer to byte array
*          length: number of bytes to read into array
* Output   number of bytes read
*/
uint8_t twi_readFrom(uint8_t address, uint8_t* data, uint8_t length, uint8_t sendStop)
{
    return length;
}
void twi_init() {}
int WDTCTL;
// ========================================================================================================= Base program
#include <CircPumpDriver.ino>



void start_simulation()
{
    //rtc_time = DateTime::getEpochFromDateTime(2017, 3, 26, 0, 50, 0);
    rtc_time = DateTime::getEpochFromDateTime(2017, 10, 29, 0, 50, 0);

    setup();
    while (true)
    {
        // 10 ticks per 1s
        for (int i = 10; i; i--) {
            loop();
        }
        rtc_time++;
    }

}