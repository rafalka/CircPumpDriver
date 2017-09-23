#pragma once
#ifndef TwoWire_h
#define TwoWire_h
#include <inttypes.h>


#define BUFFER_LENGTH 16

class TwoWire 
{
public:
    void beginTransmission(uint8_t){ }
    uint8_t endTransmission(void) { return 0; }
    size_t write(unsigned long n);
    uint8_t requestFrom(uint8_t, uint8_t);
    int read(void);
    int available(void);
};


extern TwoWire Wire;

#endif