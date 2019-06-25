#ifndef ROAD_H
#define ROAD_H

#include "LINE.h"


class ROAD
{
public:
    ROAD();
    ~ROAD();
    
    void initRoad(const Mat &binImg, int type);
    void updateLine(const Mat &binImg, Point &startPoint);
    Point BeginPoint, EndPoint;
    
    
    vector<Point> ListPoint;
    float Function[3];
    bool Status;
private:
    bool drawget_ImageLine = true;

    //TEST
    LINE lLine, rLine;
    bool test = true;
};

#endif
