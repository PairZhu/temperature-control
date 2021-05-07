#ifndef __GUI_H__
#define __GUI_H__
#include "mbed.h"
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include "Screen.h"

extern float targetT;

class GUI
{
private:
    using u8 = unsigned char;
    using uint = unsigned int;
    enum class keyCode
    {
        UP = 0,
        DOWN = 1,
        CANCEL = 2,
        ENTER = 3
    };
    static constexpr uint tableColor = Color::yellow;
    static constexpr uint targetColor = Color::red;
    static constexpr uint lineColor = Color::blue;
    static constexpr uint tableHeight = 60;
    static constexpr uint fontHeight = Font::fontHeight;
    static constexpr uint fontWidth = Font::fontWidth;
    static constexpr uint tableYMax = 50;
    static constexpr uint tableXMax = 100;
    static constexpr uint lineLeft = 3*fontWidth;
    static constexpr float targetWidth = 5.0f;

    const uint tableButtom;
    const uint lineButtom;

    Screen &screen;
    vector<uint> pointList;
    vector<uint> lastPointList;
    list<float> TList;
    float maxT;
    float minT;
    uint TTop;
    uint TButtom;
    uint pageId;

    void updateTableStr() const;
    void updateTargetLine() const;
    void updateTStr(float temperture)const;
    void drawTablePoint(uint x, uint y) const;
    void eraseTablePoint(uint x, uint y) const;
    void TargetTPage(keyCode keyValue);

    void caluRange();
    void caluPoint();
    void drawTable() const;
    uint getY(float temperature) const
    {
        return (temperature - TButtom) / (TTop - TButtom) * 50 + 0.5f;
    }

public:
    GUI(Screen &_screen)
        : screen(_screen), tableButtom(_screen.yMax - tableHeight), lineButtom(tableButtom + fontHeight),
          maxT(targetT+targetWidth), minT(targetT-targetWidth), TTop((uint(maxT) / 10 + 1) * 10),
          TButtom((uint(minT) / 10) * 10), pageId(0)
    {}
    void init() const;
    void onTChange(float newT);
    void onKeyDown(keyCode keyValue);
};

#endif // !__GUI_H__