#include "mbed.h"
#include "Thread.h"
#include <iostream>
#include <algorithm>
#include "GUI.h"
#include "TSensor.h"

SPI spi(D11, D12, D13);
Screen screen(D14,D15,D5,D6,spi);
GUI gui(screen);
TSensor tsensor(D7);
Thread TMeasurer;
float targetT=40;
// main() runs in its own thread in the OS
void PID(float newT)
{
    
}
void measureT()
{
    float lastT=0;
    while(1)
    {
        //float newT = tsensor.readT();
        float newT = lastT+(rand()%200-20)/100.0f;
        newT=(int(newT*100)%10000)/100;
        if(newT<0)
        {
            newT = lastT;
        }
        else
        {
            lastT = newT;
        }
        PID(newT);
        gui.onTChange(newT);
    }
}

int main()
{
    cout<<"Begin"<<endl;
    gui.init();
    TMeasurer.start(measureT);
    while(1);
}