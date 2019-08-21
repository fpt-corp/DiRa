#ifndef MATH_H
#define MATH_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <ros/package.h>
#include <iostream>
#define PI 3.14159265

using namespace std;
using namespace cv;

#define LEFT -1
#define RIGHT 1
#define H_TRACKING 8
#define W_TRACKING 25
#define SIZE_TRACKING 4

int fixPixelValue(int value);
int get_value_Mat_2D(const Mat &src, int y, int x);
int get_value_Mat_3D(const Mat &src, int y, int x, int j);
Rect get_Roi_track(Point inputPoint);
Rect get_Roi_tracking(Point inputPoint);  
Point get_Point_tracking(const Mat &src, Point pointTracking);
float * get_Function_Line(vector<Point> a2);
bool check_Point_in_Mat(const Mat&src, Point pointInput);
void myDraw(Mat src, vector<Point> ListPoint,const int color);
void myDraw(Mat src, Rect ROI, const int color);
void myDraw(Mat src, Point point, const int color);
float get_Angle(Point topPoint, Point botPoint);
Point giai_hpt_2_an(float a1, float b1, float c1, float a2, float b2, float c2);
Point solve(float *Function, Point A, float d);

extern Mat ImageDraw;
extern Mat ImageLine;
extern int SIZE_LANE;
#endif
