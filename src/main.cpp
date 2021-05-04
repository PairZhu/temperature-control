#include <iostream>
#include "mbed.h"
#include "GUI.h"
#include "TSensor.h"
#include "Thread.h"

// main() runs in its own thread in the OS
SPI spi(D11, D12, D13);
Screen screen(D14,D15,D5,D6,spi);
GUI gui(screen);

int main()
{
    //cout<<"\nBegin"<<endl;
    gui.init();
    
}