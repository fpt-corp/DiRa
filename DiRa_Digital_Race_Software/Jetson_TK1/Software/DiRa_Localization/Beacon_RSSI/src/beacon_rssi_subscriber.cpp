
#include "ros/ros.h"
#include "beacon_rssi/beacon.h"
#include <iostream>

void number_callback(const beacon_rssi::beacon::ConstPtr& msg) {

    ROS_INFO("rssi= %f dBm , distance = %.1f mm",msg->rssi,msg->distance);
}

int main(int argc, char **argv) { 
    ros::init(argc, argv,"beacon_subscriber");
    ros::NodeHandle node_obj;
   
    ros::Subscriber number_subscriber = node_obj.subscribe("/beacon",10,number_callback);
    ros::spin();
    return 0;
}
