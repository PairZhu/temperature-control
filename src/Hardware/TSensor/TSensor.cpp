#include "TSensor.h"
#include <algorithm>

float TSensor::measure()
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
    return response * 0.25;
}

float TSensor::readT()
{
    float newT = measure();
    if(newT<0 || abs(lastT-newT)>5)
    {
        return -1.0f;
    }
    lastT = newT;
    return newT;
}

void TSensor::init(uint initCnt)
{
    float* TArr = new float[initCnt];
    uint i = 0;
    while(i!=initCnt)
    {
        float temp = measure();
        if(temp>=0)
        {
            TArr[i]=temp;
            ++i;
        }
    }
    auto mid = TArr+initCnt/2;
    nth_element(TArr,mid,TArr+initCnt);
    lastT = *mid;
    delete [] TArr;
}