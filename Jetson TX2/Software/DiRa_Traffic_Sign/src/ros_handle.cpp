#include "ros_handle.h"
#include "detecttrafficsign.h"

void ros_handle::initTopic()
{
    depthTopic = "depth/image";
    rgbTopic = "rgb/image";
    statusTopic = "stt1";
    TFSignTopic = "TFSign_topic";
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
    pub_TFSign = nh.advertise<msgs::Int>(TFSignTopic, 1);
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

    msgs::Int TFSign_msg;
    // avoid over flow
    FID = (FID > 1000) ? FID : FID - 1000;
    // store data to msg
    TFSign_msg.frame_id = FID;
    TFSign_msg.data = TFSign;
    pub_TFSign.publish(TFSign_msg);
    // next frame id
    FID++;
}

void ros_handle::showImg()
{
    std::cout << "scc" << std::endl;
    cv::imshow("TF_rgb", rgb_image);
    cv::imshow("TF_depth", depth_image);
}

void ros_handle::process()
{
    DetectTrafficSign TFSign_obj;
    TFSign_obj.detectTrafficSign(rgb_image);
    TFSign = TFSign_obj.getTrafficSignInfor();
}