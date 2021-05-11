#ifndef __TSENSOR_H__
#define __TSENSOR_H__
#include "mbed.h"
#include "Thread.h"
#include <algorithm>

class TSensor
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    DigitalOut cs;
    SPI spi;
    float lastT;
    float measure();

public:
    static constexpr uint periodMs = 220;
    TSensor(PinName _cs, PinName* _spi)
        : spi(_spi[0],_spi[1],_spi[2],_cs,use_gpio_ssel), cs(_cs)
    {
        cs = 1;
        spi.frequency(1000000);
        spi.format(16, 1);
    }
    float getLastT() { return lastT; }
    float readT();
    void init(uint initCnt=10);
};

#endif