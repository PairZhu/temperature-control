#ifndef __GUI_H__
#define __GUI_H__
#include "mbed.h"
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include "Screen.h"

size_t getTargetT();
void setTargetT(size_t value);
extern size_t _targetT;

bool getControlFlag();
void setControlFlag(bool value);

enum class KeyCode
{
    UP = 0,
    DOWN = 1,
    ENTER = 2,
};

class GUI
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    static constexpr uint tableColor = Color::yellow;
    static constexpr uint targetColor = Color::red;
    static constexpr uint lineColor = Color::blue;
    static constexpr uint cursorColor = Color::lightgreen;
    static constexpr uint fixedColor = Color::green;
    static constexpr uint optionColor = Color::orange;
    static constexpr uint tableHeight = 60;
    static constexpr uint fontHeight = Font::fontHeight;
    static constexpr uint fontWidth = Font::fontWidth;
    
    static constexpr uint tableYMax = 50;
    static constexpr uint tableXMax = 100;
    static constexpr uint lineLeft = 4*fontWidth+2;
    static constexpr float targetWidth = 5.0f;

    const uint tableButtom;
    const uint lineButtom;
    const uint tempStrTop;
    const uint targetStrTop;
    const uint bottonTop;

    Screen& screen;
    vector<uint> pointList;
    vector<uint> lastPointList;
    list<float> TList;
    float maxT;
    float minT;
    uint TTop;
    uint TButtom;

    void updateTargetLine();
    void updateTargetTStr();
    void updateTableStr();
    void updateBotton();
    void updateTStr(float temperture);
    void drawTablePoint(uint x, uint y);
    void eraseTablePoint(uint x, uint y);
    void showOptionChar(char ch, u8 this_cursor, uint x, uint y);

    void caluRange();
    void caluPoint();
    void drawTable();

    Mutex cursorMutex;
    u8 _cursor = 0;
    u8 getCursor()
    {
        cursorMutex.lock();
        u8 res=_cursor;
        cursorMutex.unlock();
        return res;
    }
    void setCursor(u8 value)
    {
        cursorMutex.lock();
        _cursor=value;
        cursorMutex.unlock();
    }

    Mutex cursorFixedMutex;
    bool _cursorFixed = false;
    bool getCursorFixed()
    {
        cursorFixedMutex.lock();
        bool res=_cursorFixed;
        cursorFixedMutex.unlock();
        return res;
    }
    void setCursorFixed(bool value)
    {
        cursorFixedMutex.lock();
        _cursorFixed=value;
        cursorFixedMutex.unlock();
    }

    uint getY(float temperature)
    {
        return (temperature - TButtom) / (TTop - TButtom) * 50 + 0.5f;
    }

public:
    GUI(Screen &_screen)
        : screen(_screen),
          tableButtom(_screen.yMax - tableHeight), lineButtom(tableButtom + fontHeight),
          tempStrTop(tableButtom-2*fontHeight), targetStrTop(tableButtom-3*fontHeight), bottonTop(2.5*fontHeight)
    {
        const size_t targetT = _targetT;
        maxT = targetT+targetWidth;
        minT = targetT-targetWidth;
        TTop = (uint(maxT) / 10 + 1) * 10;
        TButtom = (uint(minT) / 10) * 10;
    }
    void init();
    void onTChange(float newT);
    void onKeyDown(KeyCode keyValue);
    
};

#endif // !__GUI_H__