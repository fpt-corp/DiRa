#ifndef BIENBAO_H
#define BIENBAO_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "std_msgs/Float32.h"
#include <ros/ros.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <queue>
using namespace std;
using namespace cv;


class bien_bao
{
public:
    bien_bao();
    ~bien_bao();

    void bienbao(const Mat &src);

    static int bien_b;
    
    
private:
    ros::NodeHandle bb;
    ros::Publisher bienbao_publisher;
    Mat birdViewTranform(const Mat &source);
    
};

#endif
