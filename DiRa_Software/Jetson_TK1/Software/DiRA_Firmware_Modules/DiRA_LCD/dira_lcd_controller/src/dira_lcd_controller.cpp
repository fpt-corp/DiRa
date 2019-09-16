#include <termios.h>
#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"

#include "lcd.hpp"
#include "gpio.hpp"
#include "i2c.hpp"

using namespace GPIO;


struct Point {
    int x;
    int y;
};
Point cursor;


int i2c_address;

void lcd_print(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO("Selected address is 0x%x",i2c_address);
    HitachiLCD lcd(4, 20, "/dev/i2c-1", i2c_address);    
    std::string str;
    str = msg->data;
    std::string segment;
    std::vector<std::string> array;
    std::stringstream ss(str);
    while(getline(ss, segment, ':'))
    {
        array.push_back(segment);
    }
    cursor.x = atoi(array.at(0).c_str());
    cursor.y = atoi(array.at(1).c_str());
    int n = array.at(2).length(); 
    char char_array[n+1];
    strcpy(char_array, array.at(2).c_str());
    lcd.write(cursor.x, cursor.y, char_array);
     
}
int main(int argc, char **argv)
{
    ROS_INFO("Configuring your LCD");
    ROS_INFO("Use $sudo i2c-detect -r -y 1 command to find which port is your lcd i2c address!");
    ros::init(argc, argv, "LCD_API");
    ros::NodeHandle nh("~");
    ros::Rate loop_rate(10);
    nh.param("lcd_i2c_address", i2c_address, i2c_address);
    ROS_INFO("Setting I2C address to 0x%x",i2c_address);
    ros::Subscriber sub = nh.subscribe("lcd_print", 10, lcd_print);  
    while (ros::ok())
    {
	nh.param("lcd_i2c_address", i2c_address, i2c_address);
    ros::spinOnce();
    loop_rate.sleep();
	}
    return 0;
}

