#ifndef __GUI_H__
#define __GUI_H__
#include "mbed.h"
#include "Screen.h"

class GUI
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    Screen& screen;
    u8 TArr[100];

public:
    GUI(Screen& _screen):screen(_screen) {}
    void init();
    void onTChange(u8 newT);
    void onKeyDown(u8 keyValue);


};

#endif // !__GUI_H__