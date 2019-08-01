#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include <vector>

int main(int argc, char **argv)
{
  // ROS objects
  ros::init(argc, argv, "my_publisher");
  ros::NodeHandle n;
  ros::NodeHandle m;
  ros::Publisher pub = n.advertise<std_msgs::String>("lcd_print", 10);
  ros::Publisher pub2 = m.advertise<std_msgs::Bool>("led_status", 10);
  ros::Rate loop_rate(50);
  // the message to be published
  std_msgs::StringPtr msg2(new std_msgs::String); 
  std_msgs::Bool msg; 
   while (ros::ok())
  {
    msg2->data = "2:2:xinchao";
    pub.publish(msg2);
    msg.data = true;
    pub2.publish(msg);
    msg2->data = "1:1:hello";
    pub.publish(msg2);  
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
