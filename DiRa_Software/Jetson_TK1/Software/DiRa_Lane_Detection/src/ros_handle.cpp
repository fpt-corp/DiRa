#include "ros_handle.h"

void ros_handle::initTopic()
{
    depthTopic = "depth/image";
    rgbTopic = "rgb/image";
    statusTopic = "stt3";
    detectLaneTopic = "detect_lane_topic";
}

void ros_handle::init_ros()
{
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    // Subscribers
    sub_depth = it.subscribe(depthTopic, 1, &ros_handle::depthCallback, this);
    sub_rgb = it.subscribe(rgbTopic, 1, &ros_handle::rgbCallback, this);

    // Publishers
    pub_status = nh.advertise<std_msgs::Bool>(statusTopic, 1);
    pub_detectLane = nh.advertise<msgs::Int>(detectLaneTopic, 1);
}

void ros_handle::depthCallback(const sensor_msgs::ImageConstPtr& msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, "32FC1");
        depth_image = cv_ptr->image;
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
}

void ros_handle::rgbCallback(const sensor_msgs::ImageConstPtr& msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, "8UC3");
        rgb_image = cv_ptr->image;
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
    }
}

void ros_handle::pub_done()
{
    std_msgs::Bool stt_msg;
    stt_msg.data = true;
    pub_status.publish(stt_msg); 

    // // avoid over flow
    // FID = (FID > 1000) ? FID : FID - 1000;
    // // store data to msg
    // msgs::Lane detectLane_msg;
    // detectLane_msg.frame_id = FID;

    // //store vecPoint to Lane left right
    // for(int i = 0; i < leftLane.size(); i++)
    // {
    //     detectLane_msg.left_lane.push_back( cvPoint2msgPoint(leftLane[i]) );
    //     detectLane_msg.right_lane.push_back( cvPoint2msgPoint(rightLane[i]) );
    // }
    
    // //store a, b, c var
    // detectLane_msg.a = this->a;
    // detectLane_msg.b = this->b;
    // detectLane_msg.c = this->c;
    // // publish msg
    // pub_detectLane.publish(detectLane_msg);
    // // next frame id
    // FID++;
}
 
msgs::Point ros_handle::cvPoint2msgPoint(cv::Point p)
{
    msgs::Point msg_Point;
    if (p.x == 0 && p.y == 0)
    {
        msg_Point.x = -1;
        msg_Point.y = -1;
    }
    else
    {
        msg_Point.x = p.x;
        msg_Point.y = p.y;
    }
    return msg_Point;
}

void ros_handle::showImg()
{
    std::cout <<"scc" << std::endl;
    cv::imshow("l_rgb", rgb_image);
    cv::imshow("l_depth", depth_image);
}