#include <iostream>
#include <math.h>
#include "OpenNIHelper.h"
#include <fstream>

#include <ros/ros.h>
#include <string>

#include <std_msgs/Bool.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

using namespace std;
using namespace cv;

//data msg return
int stt1 = true;
int stt2 = true;
int stt3 = true;

//topic
const string depthTopic = "depth/image";
const string rgbTopic = "rgb/image";

//callback
void callback_stt1(const std_msgs::BoolConstPtr& msg);
void callback_stt2(const std_msgs::BoolConstPtr& msg);
void callback_stt3(const std_msgs::BoolConstPtr& msg);

//convert
sensor_msgs::ImagePtr cvImage2msgImage(cv::Mat cvMat, string type);

bool ok();
void resetStatus();

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "astra_camera");

    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    // Subscribers
    ros::Subscriber sub_stt1 = nh.subscribe("stt1", 1, callback_stt1);
    ros::Subscriber sub_stt2 = nh.subscribe("stt2", 1, callback_stt2);
    ros::Subscriber sub_stt3 = nh.subscribe("stt3", 1, callback_stt3);
    
    // Publishers
    image_transport::Publisher pub_rgb = it.advertise(rgbTopic, 1);
    image_transport::Publisher pub_depth = it.advertise(depthTopic, 1);


    ni::openni2_init();
    ros::Rate rate(30.);
    Mat depth, color;
    while ( ros::ok() )
    {
        ros::spinOnce();
        ni::openni2_getmat(color, depth);
        // Mat adjMap;
        // convertScaleAbs(depth, adjMap, 255.0 / 6000);
        cout << ok() << endl;
        imshow("color", color);
        imshow("depth", depth);
        if ( true ) 
        {
            sensor_msgs::ImagePtr rgb_msg = cvImage2msgImage(color, "bgr8");
            sensor_msgs::ImagePtr depth_msg = cvImage2msgImage(depth, "16UC1");

            pub_rgb.publish(rgb_msg);
            pub_depth.publish(depth_msg);
        }
        
        

        resetStatus();
        cout << stt1 << stt2 << stt3 << endl;
        cout << ok() << endl;

        rate.sleep();
        // waitKey(1);

    }
    ni::openni2_destroy();
    cv::destroyAllWindows();
    return 0;
}

void callback_stt1(const std_msgs::BoolConstPtr& msg)
{
    stt1 = msg->data;
}

void callback_stt2(const std_msgs::BoolConstPtr& msg)
{
    stt2 = msg->data;
}

void callback_stt3(const std_msgs::BoolConstPtr& msg)
{
    stt3 = msg->data;
}

sensor_msgs::ImagePtr cvImage2msgImage(cv::Mat cvMat, string type)
{
    return cv_bridge::CvImage(std_msgs::Header(), type, cvMat).toImageMsg();
}

bool ok()
{
    return (stt1 == true && stt2 == true && stt3 == true);
}

void resetStatus()
{
    stt1 = false;
    stt2 = false;
    stt3 = false;
}
