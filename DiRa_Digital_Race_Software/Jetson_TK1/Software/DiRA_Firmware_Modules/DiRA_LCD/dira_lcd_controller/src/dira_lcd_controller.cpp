#include <termios.h>
#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"

#include <cppgpio/i2c.hpp>
#include <cppgpio/lcd.hpp>

using namespace GPIO;


struct Point {
    int x;
    int y;
};
Point cursor;

void lcd_print(const std_msgs::String::ConstPtr& msg)
{
	HitachiLCD lcd(4, 20, "/dev/i2c-1", 0x27);
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
    ros::init(argc, argv, "LCD_API");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("lcd_print", 10, lcd_print);  
    ros::spin();
    return 0;
}

