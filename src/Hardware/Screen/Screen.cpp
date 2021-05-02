#include "Screen.h"

void Screen::setAddress(uint x_beg, uint y_beg, uint x_end, uint y_end)
{
    if (horizontal)
    {
        writeCmd(0x2a); //列地址设置
        writeColor(x_beg+1);
        writeColor(x_end+1);
        writeCmd(0x2b); //行地址设置
        writeColor(y_beg+2);
        writeColor(y_end+2);
        writeCmd(0x2c); //储存器写
    }
    else
    {
        writeCmd(0x2a); //列地址设置
        writeColor(x_beg+2);
        writeColor(x_end+2);
        writeCmd(0x2b); //行地址设置
        writeColor(y_beg+1);
        writeColor(y_end+1);
        writeCmd(0x2c); //储存器写
    }
}

void Screen::fillRect(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color)
{
    setAddress(x_beg, y_beg, x_end-1, y_end-1);
    for (uint x = x_beg; x != x_end; ++x)
    {
        for (uint y = y_beg; y != y_end; ++y)
        {
            writeColor(color);
        }
    }
}

void Screen::point(uint x, uint y, uint color)
{
    setAddress(x, y, x, y);
    writeColor(color);
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