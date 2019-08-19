#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "vat_can.h"
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/opencv.hpp"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"


using namespace std;
using namespace cv;

vat_can *v;
bool button1 = false;
bool button2 = false;
bool button3 = false;
bool button4 = false;
bool ss = true;
bool old_ss = true;

void button3_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button3 = msg->data;
     if ( button3 == true){
          vat_can::b3 = true;
     }
}
void button4_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button4 = msg->data;
     if ( button4 == true){
          vat_can::start = 4;
          vat_can::co = 0;
          vat_can::b3 = false;
     }
}
void ss_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     ss = msg->data;
     
     if ( vat_can::start == 4){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               vat_can::a = 4;
               vat_can::start = 0;
               old_ss = true;
               //cout << "start"<<endl;
          }
     }
     if ( vat_can::start == 0){
          if ( ss == false ){
               vat_can::a = 0;
               vat_can::lane = 0;
               vat_can::co = 0;
               vat_can::b3 = false;
               //cout << "stop" << endl;
          } 
     }
     
}

void bienbao_Callback(const std_msgs::Float32::ConstPtr& msg){
     int bb = msg->data;
   
     if ( bb == 0 ) vat_can::bien_bao = 0;
     if ( bb == 1 ) vat_can::bien_bao = 1;
     if ( bb == 2 ) vat_can::bien_bao = 2;
	
}

void mpu_9250(const std_msgs::Float32::ConstPtr& msg){
    double a = msg->data;
    vat_can::angle = a;
    v -> vat_can::vat_can_a();
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "vat_can");
    
    v = new vat_can();
    ros::NodeHandle a;
    ros::NodeHandle m;
    ros::NodeHandle b3;
    ros::NodeHandle b4;
    ros::NodeHandle ss;
    
    ros::Subscriber sub = a.subscribe("/set_bien_bao", 10, bienbao_Callback);
    ros::Subscriber sub1 = m.subscribe("/imu_angle", 10, mpu_9250);
    ros::Subscriber sub2 = b4.subscribe("/bt4_status", 10, button4_Callback);
    ros::Subscriber sub3 = ss.subscribe("/ss_status", 10, ss_Callback);
    ros::Subscriber sub4 = b3.subscribe("/bt3_status", 10, button3_Callback);
    ros::Rate loop_rate(1000);
    ros::spin();
    destroyAllWindows();   
}


