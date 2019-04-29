#include <ros/ros.h>
#include <string>

#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <std_msgs/Float32.h>

#include "ros_handle.h"

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "car_control");

  ros_handle controller_obj;

  float speed = 10.0;
  float steer = 0.0;

  ros::Rate rate(30.);
  while (ros::ok())
  {
    ros::spinOnce();
    if (controller_obj.ok()) 
    {

    // let your sh*t goes here







      controller_obj.pub(speed, steer);
    }
    rate.sleep();
  }
}