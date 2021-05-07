#include "GUI.h"

#include <algorithm>
static inline bool Tequal(float lhs, float rhs) 
{
  return abs(lhs - rhs) < 0.01;
}

size_t GUI::getY(float temperature) const  //未完成
{
  uint TTop = (uint(maxT) / 10 + 1) * 10, TButtom = (uint(minT) / 10) * 10;
  return (temperature - TButtom) / (TTop - TButtom) * 50 + 0.5f;
}

void GUI::updateTargetLine() const 
{
  static uint lastTargetLine = 0;
  if (lastTargetLine != 0) 
  {
    screen.line(lineLeft + 1, lastTargetLine, screen.xMax, lastTargetLine,
                tableColor);
    screen.point(lineLeft, lastTargetLine);
  }
  uint targetLine = getY(targetT) + lineButtom;
  screen.line(lineLeft, targetLine, screen.xMax, targetLine, targetColor);
  lastTargetLine = targetLine;
}

void GUI::drawTablePoint(uint x, uint y) const 
{
  if (y == 0 || y > tableYMax) return;
  screen.point(x + lineLeft + 1, y + lineButtom, lineColor);
}

void GUI::eraseTablePoint(uint x, uint y) const 
{
  if (y == 0 || y > tableYMax) return;
  if (y == getY(targetT))
    screen.point(x + lineLeft + 1, y + lineButtom, targetColor);
  else
    screen.point(x + lineLeft + 1, y + lineButtom, tableColor);
}

void GUI::init() const 
{
  screen.init();
  screen.fillRect(0, screen.yMax, screen.xMax, tableButtom, tableColor);
  screen.line(lineLeft, lineButtom, lineLeft, screen.yMax);
  screen.line(lineLeft, lineButtom, screen.xMax, lineButtom);
  updateTargetLine();
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
    if(newTTop!=TTop || newTButtom!=TButtom)
    {
        TTop=newTTop;
        TButtom=newTButtom;
        updateTargetLine();
        printf("top:%d\n", TTop);
        printf("buttom:%d\n", TButtom);
    }
  lastPointList = pointList;
  pointList.clear();
  for (auto ii : TList) 
  {
    pointList.push_back((ii - TButtom) / (TTop - TButtom) * 50 + 0.5f);
  }
}

void GUI::drawTable() const {
    for (int i = 0; i < lastPointList.size(); i ++ ) 
    {
        if(lastPointList[i] != pointList[i]) 
        {
            eraseTablePoint(i, lastPointList[i]);
            drawTablePoint(i, pointList[i]);
        }
    }
}

void GUI::onTChange(float newT) 
{
  TList.push_back(newT);
  if(TList.size() > tableXMax) TList.pop_front();
  caluRange();
  caluPoint();
  drawTable();
}

void GUI::TargetTPage(keyCode keyValue)
{
    static constexpr u8 cursorMount=10;
    static u8 cursor = 1; 
    static u8 cursorFixed = 0; //0表示未固定光标
    switch (keyValue)
    {
    case keyCode::UP:
        if(cursorFixed == 0)
        {
            if(cursor!=cursorMount)
                ++cursor;
            else
                cursor=1;
            break;
        }

    case keyCode::DOWN:
        if(cursorFixed == 0)
        {
            if(cursor!=0)
                --cursor;
            else
                cursor=cursorMount;
            break;
        }  
    }
}