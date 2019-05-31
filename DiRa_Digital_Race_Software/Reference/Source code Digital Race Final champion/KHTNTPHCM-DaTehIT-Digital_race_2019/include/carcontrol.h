#ifndef CARCONTROL_H
#define CARCONTROL_H
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ros/ros.h>
#include "std_msgs/Float32.h"

#include <vector>
#include <math.h>

#include "detectlane.h"

using namespace std;
using namespace cv;

class CarControl 
{
public:
    CarControl();
    ~CarControl();
   void driverCar(DetectLane &Detectlane, float velocity, int &carStatus);

private:
    float errorAngle(const Point &dst);
    ros::NodeHandle node_obj1;
    ros::NodeHandle node_obj2;
    
    ros::Publisher steer_publisher;
    ros::Publisher speed_publisher;

    Point carPos;
    int laneSize = 0;

    float lastAngle = 0;
    int testCount = 0;


    float minVelocity = 10;
    float maxVelocity = 20;

    float preError = 0;

    float p_error;
    float i_error;
    float d_error;

    //Tốc độ đáp ứng
    float kP;
    //KI càng lớn thì ñáp ứng quá ñộ càng chậm.
    float kI;
    //KD càng lớn thì đáp ứng quá ñộ càng nhanh.
    float kD;

    int t_kP;
    int t_kI;
    int t_kD;



};

#endif