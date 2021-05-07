#include "GUI.h"

#include <algorithm>
static inline bool Tequal(float lhs, float rhs) {
  return abs(lhs - rhs) < 0.01;
}
size_t GUI::getY(float temperature) const  //未完成
{
  uint TTop = (uint(maxT) / 10 + 1) * 10, TButtom = (uint(minT) / 10) * 10;
  return (temperature - TButtom) / (TTop - TButtom) * 50 + 0.5f;
}

void GUI::updateTargetLine() const {
  static uint lastTargetLine = 0;
  if (lastTargetLine != 0) {
    screen.line(lineLeft + 1, lastTargetLine, screen.xMax, lastTargetLine,
                tableColor);
    screen.point(lineLeft, lastTargetLine);
  }
  uint targetLine = getY(targetT) + lineButtom;
  screen.line(lineLeft, targetLine, screen.xMax, targetLine, targetColor);
  lastTargetLine = targetLine;
}

void GUI::drawTablePoint(uint x, uint y) const {
  if (y == 0 || y > tableYMax) return;
  screen.point(x + lineLeft + 1, y + lineButtom, lineColor);
}

void GUI::eraseTablePoint(uint x, uint y) const {
  if (y == 0 || y > tableYMax) return;
  if (y == getY(targetT))
    screen.point(x + lineLeft + 1, y + lineButtom, targetColor);
  else
    screen.point(x + lineLeft + 1, y + lineButtom, tableColor);
}

void GUI::init() const {
  screen.init();
  screen.fillRect(0, screen.yMax, screen.xMax, tableButtom, tableColor);
  screen.line(lineLeft, lineButtom, lineLeft, screen.yMax);
  screen.line(lineLeft, lineButtom, screen.xMax, lineButtom);
  updateTargetLine();
}

void GUI::caluRange() {
  maxT = targetT + targetWidth, minT = targetT - targetWidth;
  for (auto ii : TList) {
    maxT = max(ii, maxT);
    minT = min(ii, minT);
  }

  printf("max:%d\n", int(maxT * 100));
  printf("min:%d\n", int(minT * 100));
}

void GUI::caluPoint() {
  uint TTop = (uint(maxT) / 10 + 1) * 10, TButtom = (uint(minT) / 10) * 10;
  lastPointList = pointList;
  pointList.clean();
  for (auto ii : TList) {
    pointList.push_back((ii - TButtom) / (TTop - TButtom) * 50 + 0.5f)
  }
}

void GUI::drawTable() const {
    for (int i = 0; i < lastPointList.szie(); i ++ ) {
        if(lastPointList[i] != pointList[i]) {
            eraseTablePoint(i, lastPointList[i]);
            drawTablePoint(i, pointList[i]);
        }
    }
}

void GUI::onTChange(float newT) {
  TList.push_back(newT);
  if(TList.size() > tableXMax) TList.pop_fornt();
  caluRange();
  caluPoint();
  drawTable();
}

void GUI::onKeyDown(keyCode keyValue) {
  //修改TargetT别忘了修改minT和maxT
}