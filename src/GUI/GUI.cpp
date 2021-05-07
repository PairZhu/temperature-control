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
    screen.showStr("Temp:  .  ~",3*fontWidth,tableButtom);
    screen.showStr("Target:",fontWidth,tableButtom-fontHeight);
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
    static string lastIntStr="  ";
    static string lastDecimalStr="  ";
    string intStr=to_string(int(temperture));
    string decimalStr=to_string(int(temperture*100)%100);
    if(intStr.length()<2)
        intStr='0'+intStr;
    if(decimalStr.length()<2)
        decimalStr='0'+decimalStr;
    uint x=8*fontWidth;
    for(uint i=0;i!=2;++i,x+=fontWidth)
    {
        if(intStr[i]!=lastIntStr[i])
        {
            screen.showStr(string(1,lastIntStr[i]),x,tableButtom,Color::white);
            screen.showStr(string(1,intStr[i]),x,tableButtom);
        }
    }
    x=11*fontWidth;
    for(uint i=0;i!=2;++i,x+=fontWidth)
    {
        if(decimalStr[i]!=lastDecimalStr[i])
        {
            screen.showStr(string(1,lastDecimalStr[i]),x,tableButtom,Color::white);
            screen.showStr(string(1,decimalStr[i]),x,tableButtom);
        }
    }
    lastIntStr=intStr;
    lastDecimalStr=decimalStr;
}

void updateTargetStr()
{

}

void GUI::updateTableStr() const
{
    static string lastTopStr="";
    static string lastButtomStr="";
    if(lastTopStr!="" || lastButtomStr!="")
    {
        screen.showStr(lastTopStr,0,screen.yMax,tableColor);
        screen.showStr(lastButtomStr,0,lineButtom,tableColor);
    }
    string topStr=to_string(TTop),buttomStr=to_string(TButtom);
    lastTopStr=topStr;
    lastButtomStr=buttomStr;
    screen.showStr(topStr,0,screen.yMax);
    screen.showStr(buttomStr,0,lineButtom);
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