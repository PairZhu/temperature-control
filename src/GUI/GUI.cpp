#include "GUI.h"

#include <algorithm>
#include <string>

void GUI::init()
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
    updateBotton();
}

void GUI::updateBotton()
{
    const bool controlFlag = getControlFlag();
    string str = controlFlag ? " PAUSE " : " START ";
    uint x=43;
    for(char ch:str)
    {
        showOptionChar(ch, 2, x, bottonTop);
        x+=fontWidth;
    }
}

void GUI::updateTStr(float temperture)
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

void GUI::updateTargetTStr()
{
    const size_t targetT = getTargetT();
    char tensPlace = targetT/10 + '0', onesPlace = targetT%10 + '0';
    showOptionChar(tensPlace, 0, 11*fontWidth, targetStrTop);
    showOptionChar(onesPlace, 1, 13*fontWidth, targetStrTop);
}

void GUI::showOptionChar(char ch, u8 this_cursor, uint x, uint y)
{
    const u8 cursor=getCursor();
    const bool cursorFixed=getCursorFixed();
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

void GUI::updateTargetLine()
{
    static Mutex mutex;
    mutex.lock();
    const size_t targetT = getTargetT();
    static uint lastTargetLine = 0;
    if (lastTargetLine != 0) //删除之前的目标温度线
    {
        screen.line(lineLeft + 1, lastTargetLine, screen.xMax, lastTargetLine, tableColor);
        screen.point(lineLeft, lastTargetLine);
    }
    uint targetLine = getY(targetT) + lineButtom;
    screen.line(lineLeft, targetLine, screen.xMax, targetLine, targetColor);
    lastTargetLine = targetLine;
    mutex.unlock();
}

void GUI::drawTablePoint(uint x, uint y)
{
    if (y == 0 || y > tableYMax)
        return;
    screen.point(x + lineLeft + 1, y + lineButtom, lineColor);
}

void GUI::eraseTablePoint(uint x, uint y)
{
    const size_t targetT = getTargetT();
    if (y == 0 || y > tableYMax)
        return;
    if (y == getY(targetT))
        screen.point(x + lineLeft + 1, y + lineButtom, targetColor);
    else
        screen.point(x + lineLeft + 1, y + lineButtom, tableColor);
}

void GUI::updateTableStr()
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
    const size_t targetT = getTargetT();
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

void GUI::drawTable()
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
    u8 cursor=getCursor();
    bool cursorFixed=getCursorFixed();

    static constexpr u8 optionAmount = 3;
    const size_t targetT = getTargetT();
    if(cursorFixed)
    {
        switch(keyValue)
        {
        case KeyCode::ENTER :
            setCursorFixed(false);
            if(cursor==2)
            {
                setControlFlag(false);
            }
            break;
        case KeyCode::UP :
            switch(cursor)
            {
            case 0:
                if(targetT<90)
                    setTargetT(targetT+10);
                else
                    setTargetT(targetT-70);
                break;
            case 1:
                if(targetT%10<9)
                    setTargetT(targetT+1);
                else
                    setTargetT(targetT-9);
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
                    setTargetT(targetT-10);
                else
                    setTargetT(targetT+70);
                break;
            case 1:
                if(targetT%10>=1)
                    setTargetT(targetT-1);
                else
                    setTargetT(targetT+9);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        updateTargetLine();
    }
    else
    {
        switch(keyValue)
        {
        case KeyCode::ENTER :
            setCursorFixed(true);
            if(cursor==2)
            {
                setControlFlag(true);
            }
            break;
        case KeyCode::DOWN :
            if(cursor<optionAmount-1)
                setCursor(cursor+1);
            else
                setCursor(0);
            break;
        case KeyCode::UP :
            if(cursor>0)
                setCursor(cursor-1);
            else
                setCursor(optionAmount-1);
            break;
        default:
            break;
        }
    }
    updateTargetTStr();
    updateBotton();
}