#include "GUI.h"
#include "Screen.h"
#include <string>
#define x_b 0
#define y_b 0
#define x_e 128
#define y_e 100
#define x_m 64

Screen screen_();


void GUI::init()
{
	screen_().fillRect(x_b,y_b,x_e,y_b);  //初始化
	/*  显示文本
    screen_().showStr("input goal temperature:",x_m,80);
    screen_().showStr("input K Ki Kd parameter:",x_m,60);
    screen_().showStr("input pwm parameter",x_m,20);
    */
}

void GUI::onTChange(u8 newT)
{
    
}

void GUI::onKeyDown(u8 keyValue)
{
    int ouput=0,count;
	while(ENTER==keyValue)
    {
        if(CANCEL==keyValue) break;
        else{
            if(keyValue<=1)
            {
                if(UP==keyValue)    ++ouput>9?ouput=9:ouput;
                else    --ouput<0?ouput=0:ouput;
            }
        }
    }
}
