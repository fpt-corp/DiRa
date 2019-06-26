#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "tree.h"
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "opencv2/opencv.hpp"
#include "std_msgs/Bool.h"
#include "std_msgs/String.h"


using namespace std;
using namespace cv;

tree *tr;



void imageCallback(const sensor_msgs::ImageConstPtr& msg)
    {
      Mat src; cv_bridge::CvImagePtr cv_ptr;
    
      try
      {
        //imshow("view", cv_bridge::toCvShare(msg, "bgr8")->image);
        src = cv_bridge::toCvShare(msg, "bgr8")->image;
        
        tr -> tree_a(src);
        	
	      waitKey(1);
     }
     catch (cv_bridge::Exception& e)
     {
       ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
     }
   }


int main(int argc, char **argv)
{
    ros::init(argc, argv, "tree");
    tr = new tree();
    ros::NodeHandle t;
    
    ros::Rate loop_rate(1000);
    image_transport::ImageTransport it(t);
    image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_rect_color", 1, imageCallback);
    
    //ros::Subscriber sub3 = nh1.subscribe("/camera/depth/image_raw", 1, imageDepthCallback);
    ros::spin();
    destroyAllWindows();   
}


