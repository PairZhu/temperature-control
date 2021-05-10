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
    SPI *const spi;
    const bool new_spi;
    float lastT;
    TSensor(PinName _cs, SPI *spi_ptr, bool _new_spi)
        : cs(_cs), spi(spi_ptr), new_spi(_new_spi)
    {
        cs = 1;
    }
    float measure();

public:
    static constexpr uint periodMs = 220;
    TSensor(PinName _cs, PinName so = D12, PinName sck = D13)
        : TSensor(_cs, new SPI(D11, so, sck), true) {}
    TSensor(PinName _cs, SPI &_spi)
        : TSensor(_cs, &_spi, false) {}
    ~TSensor()
    {
        if (new_spi)
            delete spi;
    }
    float getLastT() { return lastT; }
    float readT();
    void init(uint initCnt=10);
};

#endif