
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include "jetsonGPIO.h"


using namespace std;

#define BUTTON1 gpio160 //PU0
#define BUTTON2 gpio161 //PU1
#define BUTTON3 gpio162 //PU2
#define BUTTON4 gpio163 //PU3
#define LED     gpio164 //PU4
#define SENSOR1 gpio165 //PU5
#define SENSOR2 gpio166 //PU6




void set_led(const std_msgs::Bool::ConstPtr& msg)
{
	if (msg->data)
	{
        gpioSetValue(LED, on);
	}
	else
	{
		gpioSetValue(LED, off);
	}
}

bool boolConverter(unsigned int num)
{
	if(num == high)
	{
		return true;
	}
	else if(num == low)
	{
		return false;
	}
}


int main(int argc, char *argv[]){

    // Make the button and led available in user space
        gpioUnexport(BUTTON1);
	gpioUnexport(BUTTON2);
	gpioUnexport(BUTTON3);
	gpioUnexport(BUTTON4);
        gpioUnexport(LED);
	gpioUnexport(SENSOR1);
	gpioUnexport(SENSOR2);
        gpioExport(BUTTON1);
	gpioExport(BUTTON2);
	gpioExport(BUTTON3);
	gpioExport(BUTTON4);
        gpioExport(LED);
	gpioExport(SENSOR1);
	gpioExport(SENSOR2);
        gpioSetDirection(BUTTON1,inputPin) ;
	gpioSetDirection(BUTTON2,inputPin) ;
        gpioSetDirection(BUTTON3,inputPin) ;
	gpioSetDirection(BUTTON4,inputPin) ;
        gpioSetDirection(LED, outputPin)   ;
	gpioSetDirection(SENSOR1,inputPin) ;
	gpioSetDirection(SENSOR2,inputPin) ;
    // Reverse the button wiring; this is for when the button is wired
    // with a pull up resistor
    // gpioActiveLow(pushButton, true);
	
	ros::init(argc, argv, "GPIO_API");
    ros::NodeHandle nh;

    ros::Publisher but_pub_1 = nh.advertise<std_msgs::Bool>("bt1_status", 10);
    ros::Publisher but_pub_2 = nh.advertise<std_msgs::Bool>("bt2_status", 10);
    ros::Publisher but_pub_3 = nh.advertise<std_msgs::Bool>("bt3_status", 10);
    ros::Publisher but_pub_4 = nh.advertise<std_msgs::Bool>("bt4_status", 10);
    ros::Publisher sen_pub_1 = nh.advertise<std_msgs::Bool>("ss_status", 10);
    ros::Publisher sen_pub_2 = nh.advertise<std_msgs::Bool>("ss1_status", 10);
	
	ros::Subscriber led_sub = nh.subscribe("led_status", 1000, set_led);
	ros::Rate loop_rate(10);
	
	while (ros::ok())
	{
		unsigned int bt1_status = low;
		unsigned int bt2_status = low;
		unsigned int bt3_status = low;
		unsigned int bt4_status = low;
		unsigned int ss1_status = low;
		unsigned int ss2_status = low;
		
		bool STATUS1 = false;  //BUTTON1
                bool STATUS2 = false;  //BUTTON2
		bool STATUS3 = false;  //BUTTON3
		bool STATUS4 = false;  //BUTTON4
		bool STATUS5 = false;  //LED
		bool STATUS6 = false;  //SENSOR1
		bool STATUS7 = false;  //SENSOR2
		gpioGetValue(BUTTON1, &bt1_status);
		gpioGetValue(BUTTON2, &bt2_status);
		gpioGetValue(BUTTON3, &bt3_status);
		gpioGetValue(BUTTON4, &bt4_status);
		gpioGetValue(SENSOR1, &ss1_status);
		gpioGetValue(SENSOR2, &ss2_status);
	
		
		STATUS1 = boolConverter(bt1_status);
		STATUS2 = boolConverter(bt2_status);
		STATUS3 = boolConverter(bt3_status);
		STATUS4 = boolConverter(bt4_status);
		STATUS6 = boolConverter(ss1_status);
		STATUS7 = boolConverter(ss2_status);
		
		std_msgs::Bool status1, status2, status3, status4, status5, status6;
		
        status1.data = STATUS1;
        but_pub_1.publish(status1);
        status2.data = STATUS2;
        but_pub_2.publish(status2);
        status3.data = STATUS3;
        but_pub_3.publish(status3);
        status4.data = STATUS4;
        but_pub_4.publish(status4);
        status5.data = STATUS6;
        sen_pub_1.publish(status5);
	status6.data = STATUS7;
        sen_pub_2.publish(status6);
        ros::spinOnce();
    }
	

    return 0;
}


