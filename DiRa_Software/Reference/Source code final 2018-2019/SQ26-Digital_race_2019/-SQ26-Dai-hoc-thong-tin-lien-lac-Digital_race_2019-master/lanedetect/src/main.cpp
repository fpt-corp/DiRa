#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "lane.h"
#include "car.h"

#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/opencv.hpp"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"

using namespace std;
using namespace cv;

bool button1 = false;
bool button2 = false;
int a =0;
int bb = 0;
DetectLane *detect;
CarControl *car;



void imageCallback(const sensor_msgs::ImageConstPtr& msg)
    {
      Mat src; cv_bridge::CvImagePtr cv_ptr;
      try
      {
        //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
        src = cv_bridge::toCvShare(msg, "bgr8")->image;
        detect->update(src);
        
        car->driverCar(detect->getLeftLane(), detect->getRightLane());
	      waitKey(1);
     }
     catch (cv_bridge::Exception& e)
     {
       ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
     }
   }
/*
void imageDepthCallback(const sensor_msgs::ImageConstPtr& msg)
    {
      Mat src; cv_bridge::CvImagePtr cv_ptr;
      try
      {
        imshow("depth", cv_bridge::toCvShare(msg, "16UC1")->image);
        
	      waitKey(1);
     }
     catch (cv_bridge::Exception& e)
     {
       ROS_ERROR("Could not convert from '%s' to '16UC1'.", msg->encoding.c_str());
     }
   }
*/
void button1_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button1 = msg->data;
     if (button1 == true){
        a = 1;
        cout<<"chay"<<endl;
     }
     if (a == 1){
        DetectLane::imgCount++;
        //cout << DetectLane::imgCount << endl;
     }
}
void button2_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button2 = msg->data;
     if (button2 == true){
        a = 0;
        cout<<"dung lai"<<endl;
     }
     if (a == 0){
        DetectLane::imgCount = 0;
        //cout << DetectLane::imgCount << endl;
     }
}
void bienbao_Callback(const std_msgs::Float32::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     bb = msg->data;
     //cout << "bb "<<bb<<endl;
     if (bb == 0) DetectLane::phai = DetectLane::trai = 0;
     if ( bb == 1) { DetectLane::phai = 1; DetectLane::trai = 0; }
     if ( bb == 2 ) {DetectLane::phai = 0; DetectLane::trai = 1;}
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "lanedetect");
    namedWindow("Threshold", WINDOW_AUTOSIZE);
    detect = new DetectLane();
    car = new CarControl();
    ros::NodeHandle nh;
    ros::NodeHandle nh1;
    ros::NodeHandle b1;
    ros::NodeHandle b2;
    ros::NodeHandle b3;
    ros::Rate loop_rate(1000);
    //namedWindow("view");
    
    ros::Subscriber sub1 = b1.subscribe("/bt1_status", 10, button1_Callback);	
    ros::Subscriber sub2 = b2.subscribe("/bt2_status", 10, button2_Callback);
    ros::Subscriber sub3 = b3.subscribe("/set_bien_bao", 10, bienbao_Callback);
    // cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_rect_color", 1, imageCallback);
    
    //ros::Subscriber sub4 = nh1.subscribe("/camera/depth/image_raw", 1, imageDepthCallback);
    ros::spin();
    destroyAllWindows();   
}

