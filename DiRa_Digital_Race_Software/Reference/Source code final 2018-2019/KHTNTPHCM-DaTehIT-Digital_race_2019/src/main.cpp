#include <ros/ros.h>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/highgui/highgui.hpp>

#include "detectlane.h"
#include "carcontrol.h"

#include "objectdetect.h"
#include "signclassify.h"

bool STREAM = true;

VideoCapture capture("video.avi");
DetectLane detect;
CarControl *car;

int carStatus =0;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{

    cv_bridge::CvImagePtr cv_ptr;
    Mat out;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        cv::imshow("View", cv_ptr->image);
	waitKey(1);
        detect.update(cv_ptr->image);
        car->driverCar(detect, 10, carStatus);
      
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "image_listener");
    cv::namedWindow("View");
    cv::namedWindow("PID");
    cv::namedWindow("Object_View");
    //detect = new DetectLane();
    car = new CarControl();

   // cout << "--------------"<<getBuildInformation() <<endl;
    waitKey(1);
    if (STREAM) {
       // cv::startWindowThread();

        ros::NodeHandle nh;
        image_transport::ImageTransport it(nh);
        image_transport::Subscriber sub = it.subscribe("/lane_filtered", 1, imageCallback);

        ros::spin();
    }
    cv::destroyAllWindows();
}
