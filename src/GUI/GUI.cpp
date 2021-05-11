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
    screen.showStr("Temp:--.--~",5*fontWidth,tempStrTop);
    screen.showStr("Target:     ~",3*fontWidth,targetStrTop);
    updateTargetTStr();
}

void GUI::updateTStr(float temperture) const
{
    static string lastTStr="--.--~";
    string intStr=to_string(int(temperture));
    string decimalStr=to_string(int(temperture*100)%100);
    if(decimalStr.length()<2)
        decimalStr='0'+decimalStr;
    string TStr = intStr + '.' + decimalStr + '~';
    screen.refreshStr(TStr, 10*fontWidth, tempStrTop, lastTStr);
    lastTStr = TStr;
}

void GUI::updateTargetTStr() const
{
    char tensPlace = targetT/10 + '0', onesPlace = targetT%10 + '0';
    showOptionChar(tensPlace, 0, 11*fontWidth, targetStrTop);
    showOptionChar(onesPlace, 1, 13*fontWidth, targetStrTop);
}

void GUI::showOptionChar(char ch, u8 this_cursor, uint x, uint y) const
{
    if(cursor == this_cursor)
    {
        if(cursorFixed)
            screen.showChar(ch, x, y, Color::black, fixedColor);
        else
            screen.showChar(ch, x, y, Color::black, cursorColor);
    }
    else
    {
        screen.showChar(ch, x, y, Color::black, optionColor);
    }
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

void GUI::onKeyDown(KeyCode keyValue)
{
    static constexpr u8 optionAmount = 2;
    if(cursorFixed)
    {
        switch(keyValue)
        {
        case KeyCode::ENTER :
            cursorFixed = false;
            break;
        case KeyCode::UP :
            switch(cursor)
            {
            case 0:
                if(targetT<90)
                    targetT+=10;
                else
                    targetT-=70;
                break;
            case 1:
                if(targetT%10<9)
                    ++targetT;
                else
                    targetT-=9;
                break;
            default:
                break;
            }
            break;
        case KeyCode::DOWN :
            switch(cursor)
            {
            case 0:
                if(targetT>=30)
                    targetT-=10;
                else
                    targetT+=70;
                break;
            case 1:
                if(targetT%10>=1)
                    --targetT;
                else
                    targetT+=9;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch(keyValue)
        {
        case KeyCode::ENTER :
            cursorFixed = true;
            break;
        case KeyCode::UP :
            cursor = (cursor+1)%optionAmount;
            break;
        case KeyCode::DOWN :
            if(cursor>0)
                --cursor;
            else
                cursor = optionAmount - 1;
            break;
        default:
            break;
        }
    }
    updateTargetTStr();
}