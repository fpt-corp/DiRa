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
#include "detectsign.h"
#define khoang_cach_trai 100
#define khoang_cach_phai 100
#define bam_lane 300

#define h 480
#define w 640



#define MAX_STEERING 60
#define MIN_STEERING -60
using namespace std;
using namespace cv;

class CarControl 
{
public:
    CarControl();
    ~CarControl();
    void driverCar(const Mat &view, const vector<Point> left, const vector<Point> right, float velocity, int flag, bool sensor);
    

private:
    float errorAngle(const Point &dst);
    ros::NodeHandle node_obj1;
    ros::NodeHandle node_obj2;
    ros::NodeHandle n;

    ros::Publisher pub ;
    ros::Publisher steer_publisher;
    ros::Publisher speed_publisher;

    Point carPos;
    
    

    float minVelocity = 0;
    float maxVelocity = 10;
    
    float maxAngle = 60;
    float preError;
    
    float p_error;
    float i_error;
    float d_error;

    float kP;
    float kI;
    float kD;

    int t_kP;
    int t_kI;
    int t_kD;
    float dt;
};

#endif
