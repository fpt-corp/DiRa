#ifndef DETECTLANE_H
#define DETECTLANE_H

#include <ROAD.h>

using namespace std;
using namespace cv;


#define NONE 0
#define ROWS 240 
#define COLS 320 
#define X_MAX COLS-1
#define Y_MAX ROWS-1
//#define SIZE_LANE 120
#define SIZE_LINE 5

class DetectLane
{
public:
    DetectLane();
    ~DetectLane();

  
    LINE lLine, rLine, midLine;
    //
    void update(const Mat &src);    
    Mat get_ImageBirdView(const Mat &src);
    Mat get_ImageLine(const Mat &src);
    Mat get_ImageRoad(const Mat &src);
    LINE get_mid_by_right(const LINE &rightLine);
    LINE get_mid_by_left(const LINE &leftLine);
    LINE get_right_by_left(const LINE &leftLine);
    LINE get_left_by_right(const LINE &rightLine);
    Mat getRoad(const Mat &src, int y_Value);
    //void get_center_Line(const LINE &rightLine,const LINE &leftLine);
    //void get_mid_by_LeftAndRight(LINE &rightLine,const LINE &leftLine, const LINE &rightLine);
private:

    bool test = true;
};

#endif
