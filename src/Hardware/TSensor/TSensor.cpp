#include "TSensor.h"
#include <algorithm>

float TSensor::measure()
{

    spi.select();
    //cs=0;
    //cs=1;
    spi.deselect();
    
    ThisThread::sleep_for(periodMs*1ms);

    //cs=0;

    uint response = spi.write(0xff);

    //cs=1;


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
    if(newT<0 || abs(lastT-newT)>2)
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