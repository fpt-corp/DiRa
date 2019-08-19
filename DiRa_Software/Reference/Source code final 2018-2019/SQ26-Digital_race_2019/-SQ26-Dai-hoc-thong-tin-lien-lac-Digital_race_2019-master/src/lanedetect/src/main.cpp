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
bool button3 = false;
bool ss = true;
bool old_ss = true;
int l =0 ;
int a =0;
int bb = 0;
float tr = 0;
 Mat src1;
DetectLane *detect;
CarControl *car;



void imageCallback(const sensor_msgs::ImageConstPtr& msg)
    {
       Mat src;
       src1 = src.clone();
       cv_bridge::CvImagePtr cv_ptr;
      try
      {
        //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
        src = cv_bridge::toCvShare(msg, "bgr8")->image;
        detect->update(src);
        DetectLane::iscomplete = 1;
        car->driverCar(detect->getLeftLane(), detect->getRightLane());
	      waitKey(1);
     }
     catch (cv_bridge::Exception& e)
     {
       ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
     }
   }

void imageDepthCallback(const sensor_msgs::ImageConstPtr& msg_depth)
{
      cv_bridge::CvImagePtr img_ptr_depth;
      try
      {
        img_ptr_depth = cv_bridge::toCvCopy(*msg_depth, sensor_msgs::image_encodings::TYPE_16UC1);
        
      }
     catch (cv_bridge::Exception& e)
     {
        ROS_ERROR("cv_bridge exception:  %s", e.what());
        return;
     }
     
     Mat mat_depth = img_ptr_depth->image;
     //detect->update2(mat_depth);
}

void button1_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button1 = msg->data;
     if (button1 == true){
        a = 1;
     	DetectLane::imgCount = 1;   
     }
}
void button2_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button2 = msg->data;
     if (button2 == true){
	DetectLane::imgCount = 2;
	a = 1;
     }
}
void button3_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button3 = msg->data;
     if (button3 == true){
        a = 1;
	DetectLane::imgCount = 3;
     }
}
void ss_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     ss = msg->data;
     if ( a == 1 ){
        if ( ss == false) old_ss = ss;
     
         if ( ss != old_ss) 
            {
               a = 2;
               old_ss = true;
            }
         
     }
     if ( a == 2 ) {
        if ( ss == false)
        {
	   DetectLane::imgCount = 0;
           a = 0;
	   l =0;
        }
     }
}
void bienbao_Callback(const std_msgs::Float32::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     bb = msg->data;
     //cout << "bb "<<bb<<endl;
     if ( l == 0 ){
     	if (bb == 0) DetectLane::phai = DetectLane::trai = 0;
     	if ( bb == 1) { DetectLane::phai = 1; DetectLane::trai = 0; }
     	if ( bb == 2 ) {DetectLane::phai = 0; DetectLane::trai = 1;}
     }	
	
}
void lane_Callback(const std_msgs::Float32::ConstPtr& msg){
     l = msg->data;
     //cout << l << endl;
     
     if ( l == 1) { DetectLane::phai = 1; DetectLane::trai = 0; }
     if ( l == 2 ) {DetectLane::phai = 0; DetectLane::trai = 1;}
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
    ros::NodeHandle b4;
    ros::NodeHandle b5;
    ros::NodeHandle b6;
   
    ros::Rate loop_rate(1000);
    //namedWindow("view");
    
    ros::Subscriber sub1 = b1.subscribe("/bt1_status", 10, button1_Callback);	
    ros::Subscriber sub2 = b2.subscribe("/bt2_status", 10, button2_Callback);
    ros::Subscriber sub3 = b3.subscribe("/set_bien_bao", 10, bienbao_Callback);
    ros::Subscriber sub4 = b4.subscribe("/bt3_status", 10, button3_Callback);
    ros::Subscriber sub5 = b5.subscribe("/ss_status", 10, ss_Callback);
    ros::Subscriber sub6 = b6.subscribe("/set_vat_can", 10, lane_Callback);
    // cv::startWindowThread();
    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_rect_color", 1, imageCallback);
    
    //ros::Subscriber sub6 = nh1.subscribe("/camera/depth/image_raw", 2, imageDepthCallback);
    ros::spin();
    destroyAllWindows();   
}

