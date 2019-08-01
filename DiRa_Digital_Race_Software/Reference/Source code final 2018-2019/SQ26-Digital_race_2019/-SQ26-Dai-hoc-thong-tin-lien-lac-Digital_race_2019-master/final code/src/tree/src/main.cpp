#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "tree.h"
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/opencv.hpp"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"


using namespace std;
using namespace cv;

tree *tr;
bool button1 = false;
bool button2 = false;
bool button3 = false;
bool ss = true;
bool old_ss = true;
bool old_b1 = false;
bool old_b2 = false;

void button1_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button1 = msg->data;
     if ( button1 == true){
       old_b1 = button1;
     }
     if ( button1 != old_b1 && tree::set_offset==true){
          tree::offset = tree::offset + 1;
          old_b1 = false;
     }
}
void button2_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button2 = msg->data;
     if ( button2 == true){
       old_b2 = button2;
     }
     if ( button2 != old_b2 && tree::set_offset==true){
          tree::offset = tree::offset - 1;
          old_b2 = false;
     }
}
void button3_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button3 = msg->data;
    
     tree::set_offset = button3;
}

void ss_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     ss = msg->data;
     /*if ( start == 1){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               
               start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( start == 2){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
              
               start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( start == 3){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               
               start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( start == 0){
          if ( ss == false ){
               
               cout << "stop" << endl;
          } 
     }
     */
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
    {
      Mat src; cv_bridge::CvImagePtr cv_ptr;
    
      try
      {
        //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
        src = cv_bridge::toCvShare(msg, "bgr8")->image;
        
        tr -> tree_a(src);
        	
	      waitKey(1);
     }
     catch (cv_bridge::Exception& e)
     {
       ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
     }
   }

void mpu_9250(const std_msgs::Float32::ConstPtr& msg){
    double a = msg->data;
    tree::angle = a;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "tree");
    namedWindow("tree", WINDOW_AUTOSIZE);
    tr = new tree();
    ros::NodeHandle t;
    ros::NodeHandle m;
    ros::NodeHandle b1;
    ros::NodeHandle b2;
    ros::NodeHandle b3;

    ros::Rate loop_rate(1000);
    image_transport::ImageTransport it(t);
    image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_rect_color", 1, imageCallback);
    ros::Subscriber sub1 = m.subscribe("/imu_angle", 10, mpu_9250);
    ros::Subscriber sub2 = b3.subscribe("/bt3_status", 10, button3_Callback);
    ros::Subscriber sub3 = b1.subscribe("/bt1_status", 10, button1_Callback);
    ros::Subscriber sub4 = b2.subscribe("/bt2_status", 10, button2_Callback);
    //ros::Subscriber sub3 = nh1.subscribe("/camera/depth/image_raw", 1, imageDepthCallback);
    ros::spin();
    destroyAllWindows();   
}


