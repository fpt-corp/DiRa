#ifndef CARCONTROL_H
#define CARCONTROL_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ros/ros.h>
#include "std_msgs/Float32.h"

#include <vector>
#include <math.h>

#include "lane.h"

using namespace std;
using namespace cv;

class CarControl 
{
public:
    CarControl();
    ~CarControl();
    void driverCar(const vector<Point> &left, const vector<Point> &right);

private:
    float errorAngle(const Point &dst);
    ros::NodeHandle node_obj1;
    ros::NodeHandle node_obj2;
    
    ros::Publisher steer_publisher;
    ros::Publisher pos_camera_publisher;

    Point carPos;

    float laneWidth = 40;

    float minVelocity = 10;
    float maxVelocity = 50;

    float previous_error = 0;

    float kP;
    float kI;
    float kD;
    float P = 0;
    float D = 0;
    float I = 0;
    float PID_value = 0;

    int max_e;
    int t_kP;
    int t_kI;
    int t_kD;
};

#endif
