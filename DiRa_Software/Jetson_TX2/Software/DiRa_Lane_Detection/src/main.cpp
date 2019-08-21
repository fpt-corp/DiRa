#include <ros/ros.h>
#include <string>

#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <std_msgs/Float32.h>

#include "ros_handle.h"

int main(int argc, char* argv[])
{
  ros::init(argc, argv, "detect_lane");
  ros_handle detectLane_obj;

  ros::Rate rate(30.);
  while(ros::ok())
  {
    ros::spinOnce();
    detectLane_obj.showImg();
    
    // let your sh*t goes here



    detectLane_obj.pub_done();
    rate.sleep();
  }
  
}