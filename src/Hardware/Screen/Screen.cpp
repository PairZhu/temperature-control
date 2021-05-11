#include "Screen.h"

void Screen::setAddress(uint x_beg, uint y_beg, uint x_end, uint y_end)
{
    if (horizontal)
    {
        writeCmd(0x2a); //列地址设置
        writeColor(x_beg + 1);
        writeColor(x_end + 1);
        writeCmd(0x2b); //行地址设置
        writeColor(y_beg + 2);
        writeColor(y_end + 2);
        writeCmd(0x2c); //储存器写
    }
    else
    {
        writeCmd(0x2a); //列地址设置
        writeColor(x_beg + 2);
        writeColor(x_end + 2);
        writeCmd(0x2b); //行地址设置
        writeColor(y_beg + 1);
        writeColor(y_end + 1);
        writeCmd(0x2c); //储存器写
    }
}

void Screen::fillRect(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color)
{
    if (x_beg > x_end)
        swap(x_beg, x_end);
    if (y_beg > y_end)
        swap(y_beg, y_end);
    if (x_beg > xMax)
        x_beg = xMax;
    if (y_beg > yMax)
        y_beg = yMax;
    setAddress(x_beg, y_beg, x_end, y_end);
    for (uint x = x_beg; x <= x_end; ++x)
    {
        for (uint y = y_beg; y <= y_end; ++y)
        {
            writeColor(color);
        }
    }
}

void Screen::point(uint x, uint y, uint color)
{
    if (x > xMax || y > yMax)
        return;
    setAddress(x, y, x, y);
    writeColor(color);
}

void Screen::line(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color)
{
    if (x_beg == x_end || y_beg == y_end)
        fillRect(x_beg, y_beg, x_end, y_end, color);
    int x, y;
    int dx = x_end - x_beg, dy = y_end - y_beg;
    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    int p = 2 * dy - dx;
    int twody = 2 * dy, twodysubdx = 2 * (dy - dx);

    if (x_beg > x_end)
    {
        x = x_end;
        y = y_end;
        x_end = x_beg;
        y_end = y_beg;
    }
    else
    {
        x = x_beg;
        y = y_beg;
    }
    point(x, y, color);
    while (x < x_end)
    {
        x++;
        if (p < 0)
        {
            p += twody;
        }
        else
        {
            y++;
            p += twodysubdx;
        }
        point(x, y, color);
    }
}

void Screen::rect(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color)
{
    line(x_beg, y_beg, x_end, y_beg, color);
    line(x_beg, y_end, x_end, y_end, color);
    line(x_beg, y_beg, x_beg, y_end, color);
    line(x_end, y_beg, x_end, y_end, color);
}

void Screen::showChar(char ch, uint lrx, uint lry, uint color, int BKColor)
{
    if(ch==' ' && BKColor<0) return;
    const vector<u8> &chData = fontMap[ch];
    uint y = lry;
    for (u8 data : chData)
    {
        uint x = lrx;
        for (u8 i = 1; i != 1<<6; i <<= 1,++x)
        {
            if (data & i)
            {
                point(x, y, color);
            }
            else if(BKColor>=0)
            {
                point(x, y, BKColor);
            }
        }
        --y;
    }
}

void Screen::showStr(string str, uint lrx, uint lry, uint color, int BKColor)
{
    uint x=lrx;
    for(auto ch:str)
    {
        showChar(ch, x, lry, color, BKColor);
        x+=Font::fontWidth;
    }
}

void Screen::refreshStr(string newStr ,uint x, uint y, string lastStr, uint BKColor)
{
    while(newStr.size()>lastStr.size())
        lastStr+=' ';
    while(lastStr.size()>newStr.size())
        newStr+=' ';
    for(uint i=0;i!=newStr.size();++i,x+=Font::fontWidth)
    {
        if(newStr[i]!=lastStr[i])
        {
            showChar(newStr[i],x,y,brushColor,BKColor);
        }
    }
}

void Screen::init()
{
    reset = 0;
    ThisThread::sleep_for(100ms);
    reset = 1;
    ThisThread::sleep_for(100ms);
    backLight = 1;
    ThisThread::sleep_for(100ms);
    writeCmd(0x11);
    ThisThread::sleep_for(120ms);
    //设置帧率
    writeCmd(0xB1); //普通全彩模式帧率
    writeData(0x05);
    writeData(0x3C);
    writeData(0x3C);
    writeCmd(0xB2); //空闲模式帧率
    writeData(0x05);
    writeData(0x3C);
    writeData(0x3C);
    writeCmd(0xB3); //部分全彩模式帧率
    writeData(0x05);
    writeData(0x3C);
    writeData(0x3C);
    writeData(0x05);
    writeData(0x3C);
    writeData(0x3C);
    //设置翻转模式
    writeCmd(0xB4);
    writeData(0x03); //普通模式:空闲模式:部分全彩模式，点:列:列
    //电源配置
    writeCmd(0xC0);
    writeData(0x28);
    writeData(0x08);
    writeData(0x04);
    writeCmd(0xC1);
    writeData(0XC0);
    writeCmd(0xC2);
    writeData(0x0D);
    writeData(0x00);
    writeCmd(0xC3);
    writeData(0x8D);
    writeData(0x2A);
    writeCmd(0xC4);
    writeData(0x8D);
    writeData(0xEE);
    //配置显示和翻转方向 my mx mv ml rgb mh
    writeCmd(0xC5); //VCOM
    writeData(0x1A);
    writeCmd(0x36); //MX, MY, RGB mode
    if (horizontal)
        writeData(0xe0);
    else
        writeData(0x40);
    //设置灰度
    writeCmd(0xE0);
    writeData(0x04);
    writeData(0x22);
    writeData(0x07);
    writeData(0x0A);
    writeData(0x2E);
    writeData(0x30);
    writeData(0x25);
    writeData(0x2A);
    writeData(0x28);
    writeData(0x26);
    writeData(0x2E);
    writeData(0x3A);
    writeData(0x00);
    writeData(0x01);
    writeData(0x03);
    writeData(0x13);
    writeCmd(0xE1);
    writeData(0x04);
    writeData(0x16);
    writeData(0x06);
    writeData(0x0D);
    writeData(0x2D);
    writeData(0x26);
    writeData(0x23);
    writeData(0x27);
    writeData(0x27);
    writeData(0x25);
    writeData(0x2D);
    writeData(0x3B);
    writeData(0x00);
    writeData(0x01);
    writeData(0x04);
    writeData(0x13);
    //颜色模式（颜色数量）
    writeCmd(0x3A);
    writeData(0x05); //65,000种颜色
    //开始绘制页面
    writeCmd(0x29);
    clear();
}
