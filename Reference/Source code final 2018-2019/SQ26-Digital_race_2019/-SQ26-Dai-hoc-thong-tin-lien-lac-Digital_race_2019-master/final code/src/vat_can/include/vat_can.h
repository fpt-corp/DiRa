#ifndef VAT_CAN_H
#define VAT_CAN_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "std_msgs/Float32.h"
#include <ros/ros.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <unistd.h>
using namespace std;
using namespace cv;


class vat_can
{
public:
    vat_can();
    ~vat_can();
    void vat_can_a();
    static double angle;
    static int start;
    static int a;
    static int bien_bao;
    static int lane;
    static int co;
    static bool b3;
private:
    ros::NodeHandle vc;
    ros::Publisher vat_can_publisher;
   
};

#endif
