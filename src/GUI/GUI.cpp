#include "GUI.h"
#include <algorithm>
static inline bool Tequal(float lhs,float rhs)
{
    return abs(lhs-rhs)<0.01;
}
size_t GUI::getY(float temperature) const //未完成
{
    uint TTop=(uint(maxT)/10+1)*10,TButtom=(uint(minT)/10)*10;
    return (temperature-TButtom)/(TTop-TButtom)*50+0.5f;
}

void GUI::updateTargetLine() const
{
    static uint lastTargetLine=0;
    if (lastTargetLine!=0)
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
    if(y==getY(targetT))
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

void GUI::onTChange(float newT)
{
    if(newT>maxT) {maxT=newT;printf("max:%d\n",int(maxT*100));}
    if(newT<minT) {minT=newT;printf("min:%d\n",int(minT*100));}
    uint newPoint=getY(newT);
    if(pointList.size()!=tableXMax) //如果图表还未满，直接在最后画一个点即可
    {
        drawTablePoint(pointList.size(), newPoint);
        pointList.push_back(newPoint);
        TList.push_back(newT);
        return;
    }
    //如果图表已满，则整体移动像素点，并删除最前面的点
    uint x=0;
    pointList.push_back(newPoint);
    TList.push_back(newT);
    for(auto node=pointList.cbegin();x!=tableXMax;++node,++x)
    {
        auto nextNode=node;
        ++nextNode;
        if(*(nextNode)==*(node)) continue;
        eraseTablePoint(x, *node);
        drawTablePoint(x, *nextNode);
    }
    float tempT=TList.front();
    pointList.pop_front();
    TList.pop_front();
    if(Tequal(tempT, minT))
    {
        float min_T=*min_element(TList.cbegin(),TList.cend());
        if(min_T>targetT-targetWidth) min_T=targetT-targetWidth;
        minT=min_T;
        updateTargetLine();
    }
    if(Tequal(tempT, maxT))
    {
        float max_T=*max_element(TList.cbegin(),TList.cend());
        if(max_T<targetT+targetWidth) max_T=targetT+targetWidth;
        maxT=max_T;
        updateTargetLine();
    }
}

void GUI::onKeyDown(keyCode keyValue)
{

    //修改TargetT别忘了修改minT和maxT
}