#include <iostream>
#include "mbed.h"
#include "Screen.h"
#include "TSensor.h"
#include "Thread.h"

Screen screen1(D14,D15,D5,D6);
// main() runs in its own thread in the OS
int main()
{
    cout<<"\nBegin"<<endl;
    screen1.init();
    screen1.line(10,10,89,100);
    while(1);
}