#ifndef __TSENSOR_H__
#define __TSENSOR_H__
#include "mbed.h"
#include "Thread.h"

class TSensor
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    DigitalOut cs;
    SPI *const spi;
    const bool new_spi;
    TSensor(PinName _cs, SPI *spi_ptr, bool _new_spi)
        : cs(_cs), spi(spi_ptr), new_spi(_new_spi)
    {
        cs = 1;
    }

public:
    static constexpr uint periodMs = 220;
    TSensor(PinName _cs, PinName so = D12, PinName sck = D13)
        : TSensor(_cs, new SPI(D11, so, sck), true) {}
    TSensor(PinName _cs, SPI &_spi)
        : TSensor(_cs, &_spi, false) {}
    ~TSensor()
    {
        if (new_spi)
        {
            delete spi;
        }
    }
    float readT()
    {
        cs = 0;
        cs = 1;
        ThisThread::sleep_for(periodMs*1ms);
        spi->lock();
        cs = 0;
        spi->frequency(1000000);
        spi->format(16, 1);
        uint response = spi->write(0xff);
        cs = 1;
        spi->unlock();
        if (response & (1 << 2))
        {
            return -1.0f;
        }
        response &= 0xffffu;
        response &= ~(1 << 15);
        response >>= 3;
        if (response<40 || response>400)
        {
            return -1;
        }
        return response * 0.25;
    }
};

#endif