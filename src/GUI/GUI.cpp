#include "GUI.h"

#include <algorithm>
#include <string>

void GUI::init() const
{
    //画图表
    screen.init();
    screen.fillRect(0, screen.yMax, screen.xMax, tableButtom, tableColor);
    screen.line(lineLeft, lineButtom, lineLeft, screen.yMax);
    screen.line(lineLeft, lineButtom, screen.xMax, lineButtom);
    updateTargetLine();
    updateTableStr();
    //显示当前温度和目标温度
    screen.showStr("Temp:--.--~",5*fontWidth,tableButtom);
    screen.showStr("Target:  ~",3*fontWidth,tableButtom-fontHeight);
    updateTargetTStr();
}

void GUI::updateTargetTStr() const
{
    char tensPlace = targetT/10, onesPlace = targetT%10;
    
}


void GUI::updateTargetLine() const
{
    static uint lastTargetLine = 0;
    if (lastTargetLine != 0) //删除之前的目标温度线
    {
        screen.line(lineLeft + 1, lastTargetLine, screen.xMax, lastTargetLine, tableColor);
        screen.point(lineLeft, lastTargetLine);
    }
    uint targetLine = getY(targetT) + lineButtom;
    screen.line(lineLeft, targetLine, screen.xMax, targetLine, targetColor);
    lastTargetLine = targetLine;
}

void GUI::drawTablePoint(uint x, uint y) const
{
    if (y == 0 || y > tableYMax)
        return;
    screen.point(x + lineLeft + 1, y + lineButtom, lineColor);
}

void GUI::eraseTablePoint(uint x, uint y) const
{
    if (y == 0 || y > tableYMax)
        return;
    if (y == getY(targetT))
        screen.point(x + lineLeft + 1, y + lineButtom, targetColor);
    else
        screen.point(x + lineLeft + 1, y + lineButtom, tableColor);
}

void GUI::updateTStr(float temperture) const
{
    static string lastTStr="--.--~";
    string intStr=to_string(int(temperture));
    string decimalStr=to_string(int(temperture*100)%100);
    if(decimalStr.length()<2)
        decimalStr='0'+decimalStr;
    string TStr = intStr + '.' + decimalStr + '~';
    screen.refreshStr(TStr, 10*fontWidth, tableButtom, lastTStr);
    lastTStr = TStr;
}

void GUI::updateTableStr() const
{
    static string lastTopStr="";
    static string lastButtomStr="";
    string topStr=to_string(TTop),buttomStr=to_string(TButtom);
    while(topStr.size()<3)
        topStr=' '+topStr;
    topStr+='~';
    while(buttomStr.size()<3)
        buttomStr=' '+buttomStr;
    buttomStr+='~';
    screen.refreshStr(topStr, 0, screen.yMax, lastTopStr, tableColor);
    screen.refreshStr(buttomStr, 0, lineButtom, lastButtomStr, tableColor);
    lastTopStr=topStr;
    lastButtomStr=buttomStr;
}

void GUI::caluRange()
{
    maxT = targetT + targetWidth, minT = targetT - targetWidth;
    for (auto ii : TList)
    {
        maxT = max(ii, maxT);
        minT = min(ii, minT);
    }
}

void GUI::caluPoint()
{
    uint newTTop = (uint(maxT) / 10 + 1) * 10, newTButtom = (uint(minT) / 10) * 10;
    if (newTTop != TTop || newTButtom != TButtom)
    {
        TTop = newTTop;
        TButtom = newTButtom;
        updateTargetLine();
        updateTableStr();
    }
    lastPointList = pointList;
    pointList.clear();
    for (auto ii : TList)
    {
        pointList.push_back(getY(ii));
    }
}

void GUI::drawTable() const
{
    for (int i = 0; i < lastPointList.size(); ++i)
    {
        if (lastPointList[i] != pointList[i])
        {
            eraseTablePoint(i, lastPointList[i]);
            drawTablePoint(i, pointList[i]);
        }
    }
    if(lastPointList.size()<pointList.size())
    {
        uint pos=pointList.size()-1;
        drawTablePoint(pos, pointList[pos]);
    }
}

void GUI::onTChange(float newT)
{
    updateTStr(newT);
    TList.push_back(newT);
    if (TList.size() > tableXMax)
        TList.pop_front();
    caluRange();
    caluPoint();
    drawTable();
}

void GUI::TargetTPage(keyCode keyValue)
{
    static constexpr u8 cursorMount = 10;
    static u8 cursor = 1;
    static u8 cursorFixed = 0; //0表示未固定光标
    switch (keyValue)
    {
    case keyCode::UP:
        if (cursorFixed == 0)
        {
            if (cursor != cursorMount)
                ++cursor;
            else
                cursor = 1;
            break;
        }

    case keyCode::DOWN:
        if (cursorFixed == 0)
        {
            if (cursor != 0)
                --cursor;
            else
                cursor = cursorMount;
            break;
        }
    default: break;
    }
}