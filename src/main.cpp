#include "mbed.h"
#include <iostream>
#include "Thread.h"
#include "TSensor.h"
#include "Screen.h"
#include "FlashIAPBlockDevice.h"

FlashIAPBlockDevice bd(0x8079000,512000);
// main() runs in its own thread in the OS
int main()
{
    bd.init();
    const int code a;
    printf("Flash block device size: %llu\n",         bd.size());
    printf("Flash block device read size: %llu\n",    bd.get_read_size());
    printf("Flash block device program size: %llu\n", bd.get_program_size());
    printf("Flash block device erase size: %llu\n",   bd.get_erase_size());
    char* writebuffer = new char[bd.get_erase_size()];
    char* readbuffer = new char[bd.get_erase_size()];


    bd.read(readbuffer, 0, bd.get_erase_size());
    printf("%s", readbuffer);
    // Screen screen1(D14,D15,D6,D5);
    // screen1.init();
    // screen1.setBKColor(Color::red);
    // screen1.setColor(Color::red);
    // screen1.clear();
    // screen1.fillRect(0,0,5,10,Color::white);
    // while (1)
    // {
    //     TSensor sensor(D7);
    //     printf("%d\n",int(sensor.readT()*100));
    //     ThisThread::sleep_for(1s);
    // }
    while(1);
}