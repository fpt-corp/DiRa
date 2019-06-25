#ifndef TREE_H
#define TREE_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "std_msgs/Float32.h"
#include <ros/ros.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include "std_msgs/Bool.h"
#include <sstream>
#include "std_msgs/String.h"

using namespace std;
using namespace cv;


class tree
{
public:
    tree();
    ~tree();

    static double angle;
    static int start;
    static bool set_offset;
    static double offset;
    void tree_a(const Mat &src);
      
private:
    ros::NodeHandle t;
    ros::NodeHandle l;
    ros::NodeHandle n;
    ros::Publisher tree_publisher;
    ros::Publisher off_publisher;
    ros::Publisher led_publisher;
    Mat birdViewTranform(const Mat &source);
    int minThreshold[3] = {0, 0, 215};
    int maxThreshold[3] = {0, 255, 255};
};

#endif
