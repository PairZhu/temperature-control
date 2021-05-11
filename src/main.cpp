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
DigitalIn enterButton(PA_13);

DigitalOut LedRed(PC_2,1);
DigitalOut LedGreen(PC_3,1);

PwmOut Resistor(D3);
PwmOut Fanner(D2);

Thread TMeasurer;

double t_integral=0; //积分累计值

Mutex targetTMutex;
size_t _targetT=50;
size_t getTargetT()
{
    targetTMutex.lock();
    size_t res = _targetT;
    targetTMutex.unlock();
    return res;
}
void setTargetT(size_t value)
{
    targetTMutex.lock();
    _targetT = value;
    targetTMutex.unlock();
}

Mutex controlFlagMutex;
bool _controlFlag = false;
bool getControlFlag()
{
    controlFlagMutex.lock();
    bool res = _controlFlag;
    controlFlagMutex.unlock();
    return res;
}
void setControlFlag(bool value)
{
    controlFlagMutex.lock();
    _controlFlag=value;
    controlFlagMutex.unlock();
}

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
    size_t targetT=getTargetT();
    
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
    printf("kp:%d\tki:%d\tkd:%d\n",int(kp*error*10000),int(ki * t_integral*10000),int(kd * (error - last_error)*10000));
    printf("100*pid:%d\n",int(pidValue*100));
    
    // 更新上次误差
	last_error = error;
    return pidValue;
}

void measureT()
{
    while(1)
    {

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

        bool control_flag = getControlFlag();
        if(control_flag)
        {
            controlT(PID(newT));
        }
        else
        {
            t_integral = 0;
        }

    }
}

void keyScan()
{
    static int lastA = 1;
    static bool enterState=0;
    static unsigned char enterBuffer = 0;
    int newA=encodeA;
    enterBuffer = (enterBuffer<<1) | enterButton;
    if(enterBuffer == 0xff && enterState == 0)
    {
        gui.onKeyDown(KeyCode::ENTER);
        enterState = 1;
    }
    if(enterBuffer == 0x00 && enterState == 1)
    {
        enterState = 0;
    }
    if(lastA==1 && newA==0)
    {
        if(encodeB==1)
        {
            gui.onKeyDown(KeyCode::DOWN);
        }
        else
        {
            gui.onKeyDown(KeyCode::UP);
        }

    }
    lastA=newA;
}

int main()
{
    printf("Begin\n");
    gui.init();
    tsensor.init();
    TMeasurer.start(callback(measureT));    
    while(1)
    {
        keyScan();
        ThisThread::sleep_for(5ms);
    }
    cout<<"Exited"<<endl;
    return 0;
}