#ifndef DETECTLANE_H
#define DETECTLANE_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ros/ros.h>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;
using namespace cv;

class DetectLane
{
public:
    DetectLane();
    ~DetectLane();

    void update(const Mat &src);
    void update2(const Mat &src);
    
    vector<Point> getLeftLane();
    vector<Point> getRightLane();

    static int slideThickness;

    static int BIRDVIEW_WIDTH;
    static int BIRDVIEW_HEIGHT;
    static int iscomplete;
    static int VERTICAL;
    static int HORIZONTAL;

    static int trai;
    static int phai;
    static int nameTS;
	static int imgCount;
    static int imgCount1;
    static int bien_trai;
    static int bien_phai;
    static float tree;
    static int lane;
    static Point null; // 

private:
    Mat preProcess(const Mat &src);
    Mat ss;
    Mat morphological(const Mat &imgHSV);
    Mat birdViewTranform(const Mat &source);
    void fillLane(Mat &src);
  
  
    int ThresholdDepth[1] = {100};
    int minThreshold[3] = {0, 0, 220};
    int maxThreshold[3] = {179, 255, 255};
    int minShadowTh[3] = {90, 43, 36};
    int maxShadowTh[3] = {120, 81, 171};
    int minLaneInShadow[3] = {90, 43, 97};
    int maxLaneInShadow[3] = {120, 80, 171};
    int binaryThreshold = 180;

    int skyLine = 85;
    int shadowParam = 40;

    vector<Point> leftLane, rightLane, centerLane;
};

#endif
