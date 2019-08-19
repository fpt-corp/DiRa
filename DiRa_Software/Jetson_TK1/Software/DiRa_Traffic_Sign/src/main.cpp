#include <ros/ros.h>
#include <string>

#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <std_msgs/Float32.h>

#include "ros_handle.h"

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "detect_traffic_sign");
  ros_handle detectTrafficSign_obj;

  ros::Rate rate(30.);
  while(ros::ok())
  {
    ros::spinOnce();
    detectTrafficSign_obj.showImg();
    cv::imshow("test", detectTrafficSign_obj.rgb_image);

    detectTrafficSign_obj.process();

    detectTrafficSign_obj.pub_done();
    rate.sleep();
  }
  
}