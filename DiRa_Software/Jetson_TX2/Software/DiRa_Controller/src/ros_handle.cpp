#include "ros_handle.h"

void ros_handle::initTopic()
{
    TFSignTopic = "TFSign_topic";
    laneTopic = "lane_topic";
    localizeTopic = "localize_topic";
}

void ros_handle::init_ros()
{
    ros::NodeHandle nh;

    //publisher
    pub_steer = nh.advertise<std_msgs::Float32>("set_steer_car_api", 1);
    pub_speed = nh.advertise<std_msgs::Float32>("set_speed_car_api", 1);

    // Subscribers
    sub_TFSign = nh.subscribe(TFSignTopic, 1, &ros_handle::TFSignCallback, this);
    sub_lane = nh.subscribe(laneTopic, 1, &ros_handle::laneCallback, this);
    sub_localize = nh.subscribe(localizeTopic, 1, &ros_handle::localizeCallback, this);
}

void ros_handle::TFSignCallback(const msgs::Int& msg)
{
    TFSign_FID = msg.frame_id;
    TFSign = msg.data;
}

void ros_handle::laneCallback(const msgs::Lane& msg)
{
    lane_FID = msg.frame_id;

    int size = msg.left_lane.size();
    for(int i = 0; i < size; i++)
    {
        cv::Point newLeftPoint = msgPoint2cvPoint( msg.left_lane[i] );
        cv::Point newRightPoint = msgPoint2cvPoint( msg.right_lane[i] );
        
        leftLane.push_back(newLeftPoint);
        rightLane.push_back(newRightPoint);
    }
    
}

void ros_handle::localizeCallback(const msgs::Int& msg)
{
    localize_FID = msg.frame_id;
    localize = msg.data;
}

bool ros_handle::ok()
{
    return (TFSign_FID == lane_FID && lane_FID == localize_FID) ? true : false;
}

cv::Point ros_handle::msgPoint2cvPoint(msgs::Point msg)
{
    cv::Point p;
    if (msg.x != -1)
    {
        p.x = msg.x;
        p.y = msg.y;
    }
    return p;
}

void ros_handle::pub(float speed, float steer)
{
    speed_msg.data = speed;
    steer_msg.data = steer;

    pub_speed.publish(speed_msg);
    pub_steer.publish(steer_msg);
}