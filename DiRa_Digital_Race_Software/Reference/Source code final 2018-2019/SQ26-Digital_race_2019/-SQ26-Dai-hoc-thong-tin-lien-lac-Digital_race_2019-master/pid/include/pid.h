#ifndef PID_H
#define PID_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ros/ros.h>
#include "std_msgs/Float32.h"

#include <vector>
#include <math.h>

using namespace std;
using namespace cv;

class pid
{
public:
   pid();
   ~pid();

    void pid_control();
    static float error;
    static float speed;
    static int a;    

private:
    ros::NodeHandle aa;
    ros::NodeHandle bb;
    ros::Publisher steer_publisher;
    ros::Publisher speed_publisher;

    

    
};

#endif
