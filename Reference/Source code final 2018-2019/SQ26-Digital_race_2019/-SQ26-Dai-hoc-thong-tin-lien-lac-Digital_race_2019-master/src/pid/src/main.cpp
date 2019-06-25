#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "pid.h"
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
bool button4 = false;
bool ss = true;
bool old_ss = true;
bool mpu = true;
int old_tree = 0;
int l = 0;
int old_l = 0;
int ssss = 0;
pid *p;


void set_error(const std_msgs::Float32::ConstPtr& msg){
    
    float error1=msg->data;
    if ( mpu == true ){
    	pid::error = error1;
	if ( ssss == 1 )	
	pid::a = old_tree;
	p->pid_control();
    }
    //cout << error<<endl;
}

void button1_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button1 = msg->data;
     if ( button1 == true){ pid::start = 1;
		ssss = 1;
     }		
}
void button2_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button2 = msg->data;
     if ( button2 == true){ pid::start = 2;
		ssss = 1;
     }
}
void button3_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button3 = msg->data;
     if ( button3 == true){ pid::start = 3;
		ssss = 1;
     }
}
void button4_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button4 = msg->data;
     if ( button4 == true){
          pid::a = 0;
          pid::start = 0;
     }
     if ( pid::a ==0){
          p->pid_control();
     }
}
void ss_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     ss = msg->data;
     if ( pid::start == 1){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               pid::a = 1;    // cham
	       old_l = old_tree = pid::a;
               pid::start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( pid::start == 2){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               pid::a = 2;    // nhanh
	       old_l = old_tree = pid::a;
               pid::start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( pid::start == 3){
          if ( ss == false){
               old_ss = ss;
          }
          if ( ss != old_ss) {
               pid::a = 3;    // nhanh
	       old_l = old_tree = pid::a;
               pid::start = 0;
               old_ss = true;
               cout << "start"<<endl;
          }
     }
     if ( pid::start == 0){
          if ( ss == false ){
               old_l = old_tree = pid::a = 0;
		ssss = 0;
               cout << "stop" << endl;
          } 
     }
	
     p->pid_control();
}
void tree_Callback(const std_msgs::Float32::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     float tr = msg->data;
     if ( tr != 100 ) mpu = false;
     else mpu = true;
     if ( mpu == false ){	
     pid::error = tr;
     if ( ssss == 1 )
     pid::a = 5;
     p->pid_control();
     }
}
void lane_Callback(const std_msgs::Float32::ConstPtr& msg){
     l = msg->data;
     //cout << l << endl;
     if ( l == 0 ) pid::a = old_l;
     if ( l == 1 ) pid::a = 4;
     if ( l == 2 ) pid::a = 4;
     if ( l == 4 ) pid::a = old_l;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pid");
    
    ros::NodeHandle a;
    ros::NodeHandle b1;
    ros::NodeHandle b2;
    ros::NodeHandle b3;
    ros::NodeHandle b4;
    ros::NodeHandle b5;
    ros::NodeHandle b6;
    ros::NodeHandle ss;
    
    p = new pid();
    
    ros::Rate loop_rate(1000);
    ros::Subscriber sub = a.subscribe("/set_error", 10, set_error);
    
    ros::Subscriber sub1 = b1.subscribe("/bt1_status", 10, button1_Callback);	
    ros::Subscriber sub2 = b2.subscribe("/bt2_status", 10, button2_Callback);
    ros::Subscriber sub3 = b3.subscribe("/bt3_status", 10, button3_Callback);
    ros::Subscriber sub4 = b4.subscribe("/bt4_status", 10, button4_Callback);
    ros::Subscriber sub5 = b5.subscribe("/set_tree", 10, tree_Callback);
    ros::Subscriber sub6 = ss.subscribe("/ss_status", 10, ss_Callback);	
    ros::Subscriber sub7 = b6.subscribe("/set_vat_can", 10, lane_Callback);
    ros::spin();
    destroyAllWindows();   
}

