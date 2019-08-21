#include <termios.h>
#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"
#include "api_hal.h"
#include "geometry_msgs/Point.h"
using namespace std;
bool STATUS1 = false;
bool STATUS2 = false;
bool STATUS3 = false;
bool STATUS4 = false;
bool STATUS5 = false;
bool STATUS6 = false;
ApiHAL HAL;
LCDI2C *lcd = new LCDI2C();
I2C *i2c_device = new I2C();
struct Point {
    int x;
    int y;
};
Point cursor;
ros::Subscriber* sub;
void set_led(const std_msgs::Bool::ConstPtr& msg)
{
    STATUS6 = msg->data;
	if (STATUS6)
	{
        HAL.gpioSetValue(LED_PIN, true);
	}
}
void lcd_print(const std_msgs::String::ConstPtr& msg){
    std::string str;
    str = msg->data;
    string segment;
    vector<string> array;
    stringstream ss(str);
    while(getline(ss, segment, ':'))
    {
        array.push_back(segment);
    }
    cursor.x = atoi(array.at(0).c_str());
    cursor.y = atoi(array.at(1).c_str());
    int n = array.at(2).length(); 
    char char_array[n+1];
    strcpy(char_array, array.at(2).c_str());
    HAL.setCursor(cursor.x,cursor.y);
    HAL.print(char_array);
     
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "hal_api");
    unsigned char lcd_adr;
    ros::NodeHandle nh1;
    ros::NodeHandle nh2;
    ros::NodeHandle nh3;
    ros::NodeHandle nh4;
    ros::NodeHandle nh5;
    ros::NodeHandle nh6;
    ros::NodeHandle sb;
    ros::NodeHandle settings_nh_;    
    ros::Publisher button1_status;
    ros::Publisher button2_status;
    ros::Publisher button3_status;
    ros::Publisher button4_status;
    ros::Publisher sensor_status;
    sub = new ros::Subscriber;
    *sub = sb.subscribe("lcd_print", 10, lcd_print);
    ros::Subscriber sub2 = nh6.subscribe("led_status", 1000, set_led);
    button1_status = nh1.advertise<std_msgs::Bool>("bt1_status",10);
    button2_status = nh2.advertise<std_msgs::Bool>("bt2_status",10);
    button3_status = nh3.advertise<std_msgs::Bool>("bt3_status",10);
    button4_status = nh4.advertise<std_msgs::Bool>("bt4_status",10);
    sensor_status = nh5.advertise<std_msgs::Bool>("ss_status",10);
    
	std_msgs::Bool status1;
    std_msgs::Bool status2;
    std_msgs::Bool status3;
    std_msgs::Bool status4;
    std_msgs::Bool status5;  
    int temp_int;
    settings_nh_.getParam("lcd_adr", temp_int);
    lcd_adr = (unsigned char) temp_int;
    i2c_device->m_i2c_bus = 1;
    HAL.initPin(lcd_adr);
    
    ros::Rate r(10);
    while (1){
        unsigned int bt1_status = -1;
        unsigned int bt2_status = -1;
        unsigned int bt3_status = -1;
        unsigned int bt4_status = -1;
        unsigned int ss1_status = -1;
        HAL.gpioGetValue(SW1_PIN, &bt1_status);
        HAL.gpioGetValue(SW2_PIN, &bt2_status);
        HAL.gpioGetValue(SW3_PIN, &bt3_status);
        HAL.gpioGetValue(SW4_PIN, &bt4_status);
        HAL.gpioGetValue(SS1_PIN, &ss1_status);

        if (bt1_status == HIGH){
            STATUS1 = true;          
        }
        else{
            STATUS1 = false;
        }
        if (bt2_status == HIGH){
            STATUS2 = true;      
        }
        else{
            STATUS2 = false;
        }
        if (bt3_status == HIGH){
            STATUS3 = true;
        }
        else{
            STATUS3 = false;
        }
        if (bt4_status == HIGH){
            STATUS4 = true;
        }
        else{
            STATUS4 = false;
        }
        if (ss1_status == HIGH){
            STATUS5 = true;
        }
        else{
            STATUS5 = false;
        }
        status1.data = STATUS1;
        button1_status.publish(status1);
        status2.data = STATUS2;
        button2_status.publish(status2);
        status3.data = STATUS3;
        button3_status.publish(status3);
        status4.data = STATUS4;
        button4_status.publish(status4);
        status5.data = STATUS5;
        sensor_status.publish(status5);
        ros::spinOnce();
    }
	return 0;
}

