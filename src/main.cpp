#include "mbed.h"
#include "Thread.h"
#include <iostream>
#include <algorithm>
#include "GUI.h"
#include "TSensor.h"

PinName spi[]={D11, D12, D13, NC};
Screen screen(D14,D15,D5,D6,spi);
TSensor tsensor(D7,spi);
GUI gui(screen);

DigitalIn encodeB(PB_7);
DigitalIn encodeA(PC_13);
DigitalOut LedRed(PC_2,1);
DigitalOut LedGreen(PC_3,1);

PwmOut Resistor(D3);
PwmOut Fanner(D2);

bool spiLock = false;

Thread TMeasurer;

Mutex threadMutex;

bool controlFlag = true;

size_t targetT=50;
double t_integral=0; //积分累计值

// main() runs in its own thread in the OS
void controlT(float pidValue)
{
    float pwmValue = pidValue; //占空比（-100% ~ +80%）
    //pwm饱和值
    if(pwmValue<0) pidValue*=10.0f;
    if(pwmValue>1) pidValue=0.8f;
    if(pwmValue<-1) pidValue=-1.0f;

    if(pidValue>0)
    {
        Fanner = 0;
        Resistor = pidValue;
    }
    else 
    {
        Resistor=0;
        Fanner = -pidValue;
    }
}

float PID(float newT)
{
    constexpr double kp = 0.1;
    constexpr double ki = 0.000176;
    constexpr double kd = 0.13636;
    
    static double last_error = 0;   //上次的温差
    double pidValue = 0;    //pid计算结果
	double error = targetT - newT; //本次测量的温差
    if((t_integral*ki<1 && error>0) || (t_integral*ki>-1 && error<0)) //积分值饱和
    {
        t_integral += error;
    }
    // 计算pid调控值
	pidValue += kp * error;
	pidValue += ki * t_integral;
	pidValue += kd * (error - last_error);
    
    //调试
    //printf("kp:%d\tki:%d\tkd:%d\n",int(kp*error*10000),int(ki * t_integral*10000),int(kd * (error - last_error)*10000));
    //printf("100*pid:%d\n",int(pidValue*100));
    
    // 更新上次误差
	last_error = error;
    return pidValue;
}

void measureT()
{
    while(1)
    {
        threadMutex.lock();
        float newT = tsensor.readT();
        if(newT<0)
        {
            printf("Error Temperature:%d !!!\n",int(newT));
            newT = tsensor.getLastT();
        }
        gui.onTChange(newT);
        if(abs(newT-40)<=1)
            LedGreen=0;
        else
            LedGreen=1;

        if(abs(newT-60)<=1)
            LedRed=0;
        else
            LedRed=1;

        bool control_flag = controlFlag;
        if(control_flag)
        {
            controlT(PID(newT));
        }
        else
        {
            t_integral = 0;
        }
        threadMutex.unlock();
    }
}

int main()
{
    cout<<"Begin"<<endl;
    gui.init();
    tsensor.init();
    TMeasurer.start(callback(measureT));
    int lastA = 1;
    while(1)
    {
        int newA=encodeA;
        if(lastA==1 && newA==0)
        {
            threadMutex.lock();
            if(encodeB==1)
            {
                printf("Down\n");
                gui.onKeyDown(KeyCode::DOWN);
            }
            else
            {
                printf("Up\n");
                gui.onKeyDown(KeyCode::UP);
            }
            threadMutex.unlock();
        }
        lastA=newA;
        ThisThread::sleep_for(5ms);
    }
    cout<<"Exited"<<endl;
    return 0;
}