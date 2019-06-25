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

using namespace std;
using namespace cv;


class tree
{
public:
    tree();
    ~tree();

    void tree_a(const Mat &src);
      
private:
    ros::NodeHandle t;
    ros::Publisher tree_publisher;
    Mat birdViewTranform(const Mat &source);
    
};

#endif
