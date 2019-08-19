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
bool ss = true;

pid *p;

void set_error(const std_msgs::Float32::ConstPtr& msg){
    
    float error1=msg->data;
    pid::error = error1;
    p->pid_control();
    //cout << error<<endl;
}

void button1_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button1 = msg->data;
     if (button1 == true){
        pid::a = 1;
     }
     if (pid::a == 1){ 
	p->pid_control();  
     }
}
void button2_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button2 = msg->data;
     if (button2 == true){
        pid::a = 0;
     }
     if (pid::a == 0){ 
	p->pid_control();  
     }
}
void button3_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     button3 = msg->data;
     if (button3 == true){
        pid::a = 2;
     }
     if (pid::a == 2){ 
	p->pid_control();  
     }
}
void ss_Callback(const std_msgs::Bool::ConstPtr& msg){
     //ROS_INFO("I heard: [%s]", msg->data.c_str());
     ss = msg->data;
     if (ss == false){
        pid::a = 0;
        cout<<"dung lai"<<endl;
     }
     if (pid::a == 0){
        p->pid_control();
     }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pid");
    
    ros::NodeHandle a;
    ros::NodeHandle b1;
    ros::NodeHandle b2;
    ros::NodeHandle b3;
    ros::NodeHandle ss;
    p = new pid();
    
    ros::Rate loop_rate(1000);
    ros::Subscriber sub = a.subscribe("/set_error", 10, set_error);
    
    ros::Subscriber sub1 = b1.subscribe("/bt1_status", 10, button1_Callback);	
    ros::Subscriber sub2 = b2.subscribe("/bt2_status", 10, button2_Callback);
    ros::Subscriber sub3 = b3.subscribe("/bt3_status", 10, button3_Callback);
    ros::Subscriber sub4 = ss.subscribe("/ss_status", 10, ss_Callback);	
    ros::spin();
    destroyAllWindows();   
}

