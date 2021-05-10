#ifndef __SCREEN_H__
#define __SCREEN_H__
#include "mbed.h"
#include "Thread.h"
#include <iostream>
#include "FontLib.h"

namespace Color
{
    enum
    {
        white = 0xffff,
        black = 0x0000,
        gray = 0x8430,
        lightblue = 0x7d7c,
        blue = 0x001f,
        darkblue = 0x01cf,
        red = 0xf800,
        lightgreen = 0x841f,
        green = 0x07e0,
        darkgreen = 0x0064,
        brown = 0xbc40,
        yellow = 0xffe0,
        cyan = 0x7fff,
        orange = 0xffa5
    };
}

class Screen
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    DigitalOut cs;
    DigitalOut reset;
    DigitalOut dataCmd;
    DigitalOut backLight;
    SPI *const spi;
    const bool new_spi;
    const bool horizontal; //水平显示
    uint brushColor;       //当前画笔颜色
    uint backgroundColor;  //背景色
    void write(u8 data)    //写入8位
    {
        spi->lock();
        cs = 0;
        spi->frequency(1000000);
        spi->format(8, 0);
        spi->write(data);
        cs = 1;
        spi->unlock();
    }
    void writeCmd(u8 cmd)
    {
        dataCmd = 0;
        write(cmd);
    }
    void writeData(u8 data)
    {
        dataCmd = 1;
        write(data);
    }
    void writeColor(uint color)
    {
        dataCmd = 1;
        write(color >> 8);
        write(color);
    }
    void setAddress(uint x_beg, uint y_beg, uint x_end, uint y_end);
    Screen(PinName _reset, PinName _dataCmd, PinName _cs, PinName _backLight, SPI *spi_ptr, bool _new_spi, bool _horizontal)
        : reset(_reset), dataCmd(_dataCmd), backLight(_backLight),
          spi(spi_ptr), cs(_cs), new_spi(_new_spi),
          horizontal(_horizontal), brushColor(Color::black), backgroundColor(Color::white),
          xMax(_horizontal?length-1:width-1),
          yMax(_horizontal?width-1:length-1)
    {
        cs = 1;
    }

public:
    static constexpr uint width = 128;
    static constexpr uint length = 160;
    const uint xMax;
    const uint yMax;
    void init();
    void setColor(uint color) { brushColor = color; }
    void setBKColor(uint color) { backgroundColor = color; }
    SPI *getSpi() { return spi; }
    uint getColor() { return brushColor; }
    uint getBKColor() { return backgroundColor; }
    void clear()
    {
        fillRect(0, 0, xMax, yMax, backgroundColor);
    }
    void fillRect(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color);
    void fillRect(uint x_beg, uint y_beg, uint x_end, uint y_end) { fillRect(x_beg, y_beg, x_end, y_end, brushColor); }
    void rect(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color);
    void rect(uint x_beg, uint y_beg, uint x_end, uint y_end) { rect(x_beg, y_beg, x_end, y_end, brushColor); }
    void point(uint x, uint y, uint color);
    void point(uint x, uint y) { point(x, y, brushColor); }
    void line(uint x_beg, uint y_beg, uint x_end, uint y_end, uint color);
    void line(uint x_beg, uint y_beg, uint x_end, uint y_end) { line(x_beg, y_beg, x_end, y_end, brushColor); }
    void showChar(char ch, uint lrx, uint lry, uint color, int BKColor=-1);
    void showChar(char ch, uint lrx, uint lry) {showChar(ch,lrx,lry,brushColor);}
    void showStr(string str, uint lrx, uint lry, uint color, int BKColor=-1);
    void showStr(string str, uint lrx, uint lry) {showStr(str,lrx,lry,brushColor);}
    void refreshStr(string newStr ,uint x, uint y, string lastStr, uint BKColor = Color::white);
    Screen(PinName _reset, PinName _dataCmd, PinName _cs, PinName _backLight, bool _horizontal = false, PinName sck = D13)
        : Screen(_reset, _dataCmd, _cs, _backLight, new SPI(D11, NC, sck), true, _horizontal) {}
    Screen(PinName _reset, PinName _dataCmd, PinName _cs, PinName _backLight, SPI &_spi, bool _horizontal = false)
        : Screen(_reset, _dataCmd, _cs, _backLight, &_spi, false, _horizontal) {}
};

#endif