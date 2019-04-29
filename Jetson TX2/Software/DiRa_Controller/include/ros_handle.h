#ifndef ros_handle_H
#define ros_handle_H
#include <ros/ros.h>
#include <string>
#include <iostream>
#include <vector>

#include <std_msgs/Float32.h>
#include <msgs/Point.h>
#include <msgs/Int.h>
#include <msgs/Lane.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>


using namespace std;

class ros_handle
{
private:
    // farme ID
    int TFSign_FID = 0;
    int lane_FID = 0;
    int localize_FID = 0;

    //Topic name
    string TFSignTopic;
    string laneTopic;
    string localizeTopic;
    void initTopic();


    //convert
    cv::Point msgPoint2cvPoint(msgs::Point msg);

    //msgs for publishing
    std_msgs::Float32 speed_msg;
    std_msgs::Float32 steer_msg;

    // Publishers
    ros::Publisher pub_steer;
    ros::Publisher pub_speed;

    // Subscribers
    ros::Subscriber sub_TFSign;
    ros::Subscriber sub_lane;
    ros::Subscriber sub_localize;
public:
    ros_handle();
    // init
    void init_ros();

    //callback
    void TFSignCallback(const msgs::Int& msg);
    void laneCallback(const msgs::Lane& msg);
    void localizeCallback(const msgs::Int& msg);

    //data msg return
    int TFSign; //this for traffic sign
    vector<cv::Point> leftLane, rightLane; // this for left lane and right lane
    int localize; // this for localize

    //check sync
    bool ok();

    // publish to car
    void pub(float speed, float steer);
    
};

ros_handle::ros_handle()
{
    initTopic();
    init_ros();
}
#endif