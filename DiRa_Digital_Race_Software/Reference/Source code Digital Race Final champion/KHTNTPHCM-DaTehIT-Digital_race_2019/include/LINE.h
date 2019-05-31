#ifndef LINE_H
#define LINE_H

#include "MATH.h"


class LINE
{
public:
    LINE();
    ~LINE();
    bool init_Line(int type);
    void get_ListPoint(const Mat &binImg, Point &startPoint);
    Point get_Point_angle();
    Point BeginPoint, EndPoint;
    
    void clear();
    
    vector<Point> ListPoint;
    float *Function;
    bool Status;
    int get_X_by_Y(int x);
    // int fy(int y);
private:
    Mat imgTestDraw;


    //tracking


};

void test();
#endif
