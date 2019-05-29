#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/Bool.h"
#include<ros/package.h>
#include "sensor_msgs/CompressedImage.h"
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include "std_msgs/String.h"

#include "detectlane.h"
#include "carcontrol.h"
#include "detectsign.h"
//#include "detectobject.h"


using namespace sensor_msgs;
using namespace message_filters;


bool STREAM = true;
int speed = 12;
bool sttBtn2;
bool preSttBtn2;
bool sttBtn3;
bool preSttBtn3;

DetectLane *detect;
CarControl *car;
DetectSign *sign;
//DetectObject *object;

bool flagDis = false;
int _turn;

int countSign = 0;
bool flagSign = false;
bool sensor = true;
Point dis(0,0);
string path = ros::package::getPath("team515");
string model = path + "/model/svm.xml";


void sensorCallback(const std_msgs::Bool::ConstPtr& msg)
{
    if( msg->data)
	{
		sensor = true;
	}
    else
	{
		sensor = false;
	}

}
void statusBtn1(const std_msgs::Bool::ConstPtr& msg)
{	
	if(msg->data){
		speed = 0;
	}
}
void statusBtn2(const std_msgs::Bool::ConstPtr& msg)
{	
	sttBtn2 = msg->data;
	if(sttBtn2 != preSttBtn2 && sttBtn2== true){
		if(speed<10)
			speed=10;
		else speed += 2;
	}
	preSttBtn2 = sttBtn2;
}
void statusBtn3(const std_msgs::Bool::ConstPtr& msg)
{	
	sttBtn3 = msg->data;
	if(sttBtn3 != preSttBtn3 && sttBtn3== true){
		if(speed<=10)
			speed=0;
		else speed -= 2;
	}
	preSttBtn3 = sttBtn3;
}
void statusBtn4(const std_msgs::Bool::ConstPtr& msg)
{	
	if(msg->data){
		speed = 10;
	}
}
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    
    cv_bridge::CvImagePtr cv_ptr;
    Mat view;
    
    try
    {	
	
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        
        view = cv_ptr->image.clone();
        
        _turn = sign->update(cv_ptr->image);

        detect->update(cv_ptr->image);

        

        if(_turn == 1 || _turn == 2) {
           
            rectangle(view, sign->drawSign(), Scalar(0,0,255));
            putText(view, ((_turn == 1)?"turn_left":"turn_right"),Point(sign->drawSign().x,sign->drawSign().y),CV_FONT_HERSHEY_COMPLEX_SMALL,0.8,Scalar(0,0,255));
	    dis.x = sign->drawSign().x;
	    dis.y = sign->drawSign().y;
	    flagDis = true;
	    flagSign = true;

        }
	else
		{
			flagDis = false;
			flagSign = false;
		}
        
        imshow("View",cv_ptr->image);
        
	car->driverCar(view,detect->getLeftLanes(), detect->getRightLanes(), speed,_turn, sensor);
	
        waitKey(1);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}
/*void depthCallback(const sensor_msgs::Image::ConstPtr& msg)
  {
	cv_bridge::CvImagePtr cv_ptr;
	try{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
		Mat depth = Mat(cv_ptr->image.size(), cv_ptr->image.type()).clone();
		depth = cv_ptr->image.clone();
		//normalize(depth, depth, 0, 1, NORM_MINMAX);
		//depth.convertTo(depth, CV_8U, 1.0/255); 
		float dist ;
		if(flagDis)
 			dist = depth.at<float>(dis.x,dis.y);
		else dist = -1;
		cout << "\nDistance: " << dist << endl;
		imshow("Depth", depth);
		waitKey(1);
	}
	catch(cv_bridge::Exception& e)
    	{
        	ROS_ERROR("'%s'", e.what());
		return;
    	}

}*/
/*void depthCallback2(const sensor_msgs::Image::ConstPtr& msg) {

    // Get a pointer to the depth values casting the data
    // pointer to floating point
    
    if(flagSign){
    float* depths = (float*)(&msg->data[0]);
    // Image coordinates of the center pixel
    int u = dis.x;
    int v = dis.y;

    // Linear index of the center pixel
    int centerIdx = u + msg->width * v;

    // Output the measure
    ROS_INFO("Center distance : %g m", depths[centerIdx]);
    }
}*/
/*void callback(const sensor_msgs::ImageConstPtr& msg_rgb , const sensor_msgs::ImageConstPtr& msg_depth)
{
	cv_bridge::CvImagePtr img_ptr_rgb;
        cv_bridge::CvImagePtr img_ptr_depth;
    try{
        img_ptr_depth = cv_bridge::toCvCopy(*msg_depth, sensor_msgs::image_encodings::TYPE_32FC1);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception:  %s", e.what());
        return;
    }
    try{
        img_ptr_rgb = cv_bridge::toCvCopy(*msg_rgb, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception:  %s", e.what());
        return;
    }

    
    imshow("Depth",img_ptr_depth->image);
    imshow("View",img_ptr_rgb->image);
    
}*/

int main(int argc, char **argv)
{
    ros::init(argc, argv, "image_listener");
    //cv::namedWindow("Sign");
    //cv::namedWindow("Threshold");
    //cv::namedWindow("PID");
    //cv::namedWindow("Sign Binary");
    cv::namedWindow("View");
    cv::namedWindow("Direct");
    //cv::namedWindow("Depth");
    detect = new DetectLane();
    car = new CarControl();
    sign = new DetectSign(model);		
    //object = new DetectObject(path);
    cv::startWindowThread();
    if (STREAM) {

        ros::NodeHandle nh;
        image_transport::ImageTransport it(nh), dp(nh), dp2(nh);
        //ros::Subscriber restart_publisher;
	ros::NodeHandle ss;
	ros::Subscriber sub_ss = ss.subscribe("/ss_status", 10, sensorCallback);
	ros::Subscriber bt1, bt2, bt3, bt4;
	ros::NodeHandle btn1, btn2, btn3, btn4;

	bt1 = btn1.subscribe("/bt1_status",1,statusBtn1);
	bt2 = btn2.subscribe("/bt2_status",1,statusBtn2);
	bt3 = btn3.subscribe("/bt3_status",1,statusBtn3);
	bt4 = btn4.subscribe("/bt4_status",1,statusBtn4);
        image_transport::Subscriber sub = it.subscribe("/camera/rgb/image_raw", 1, imageCallback);
	//image_transport::Subscriber sub_depth = dp.subscribe("/camera/depth/image_raw", 1,depthCallback);
	//image_transport::Subscriber sub_depth2 = dp2.subscribe("/camera/depth_registered/image_raw", 1,depthCallback2);


        ros::spin();
    } 
    cv::destroyAllWindows();
}
