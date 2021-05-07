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
PwmOut Resistor(D3);
PwmOut Fanner(D2);

float targetT=40;

double kp = 0.07;
double ki = 0.001;
double kd = 0.01;
double t_integral=0;

// main() runs in its own thread in the OS
void PID(float newT)
{
    static double last_error = 0;
    double res = 0;
	double error = targetT - newT;
	res += kp * error;
	t_integral += error;
	res += ki * t_integral * TSensor::periodMs;
	res += kd * (error - last_error) / TSensor::periodMs;
	last_error = error;
	
    if(res>1) res=1.0f;
    if(res<1) res=-1.0f;

    if(res>0)
    {
        Resistor.write(res);
    }
    else 
    {
        Fanner.write(-res);
    }
    
    
}
void measureT()
{
    float lastT=0;
    while(1)
    {
        float newT = tsensor.readT();
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
    Resistor.period(0.01);
    Fanner.period(0.01);
    TMeasurer.start(measureT);
    while(1) ThisThread::sleep_for(10s);
}