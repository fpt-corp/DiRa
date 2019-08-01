#include <RTIMULib.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include "std_msgs/String.h"
#include <sstream>
#include "std_msgs/Float32.h"
#include <iostream>
#include <math.h>
using namespace std;
static const double G_TO_MPSS = 9.80665;
static const double toadodau = 261;
static double mmm = 0;

int main(int argc, char **argv)
{
	int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;
    ros::init(argc, argv, "rtimulib_node");
    ROS_INFO("Imu driver is now running");
    ros::NodeHandle nh("~");
    ros::NodeHandle nh2;
    ros::NodeHandle nh3;
    ros::NodeHandle nh4;
    std::string calibration_file_path  ;
    if(!nh.getParam("calibration_file_path", calibration_file_path))
    {
        ROS_ERROR("The calibration_file_path parameter must be set to use a "
                  "calibration file.");
       // ROS_BREAK();
    }
	
    std::string calibration_file_name = "RTIMULib";
    if(!nh.getParam("calibration_file_name", calibration_file_name))
    {
        ROS_WARN_STREAM("No calibration_file_name provided - default: "
                        << calibration_file_name);
    }

    std::string frame_id = "imu_link";
    if(!nh.getParam("frame_id", frame_id))
    {
        ROS_WARN_STREAM("No frame_id provided - default: " << frame_id);
    }
    
    ros::Publisher imu_pub = nh.advertise<sensor_msgs::Imu>("imu", 1);
    ros::Publisher angle_pub = nh2.advertise<std_msgs::Float32>("imu_angle", 1);
    ros::Publisher lcd_pub = nh3.advertise<std_msgs::String>("lcd_print", 10);
    ros::Publisher lcd_pub_1 = nh4.advertise<std_msgs::String>("lcd_print_2", 10);
    ros::Rate loop_rate(1000);
    // Load the RTIMULib.ini config file
    RTIMUSettings *settings = new RTIMUSettings(calibration_file_path.c_str(),
                                                calibration_file_name.c_str());
	
    RTIMU *imu = RTIMU::createIMU(settings);
	cout << calibration_file_path.c_str() << endl;
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL))
    {
        ROS_ERROR("No Imu found");
        ROS_BREAK();
    }

    // Initialise the imu object
    imu->IMUInit();

    // Set the Fusion coefficient
    imu->setSlerpPower(0.02);
    // Enable the sensors
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    sensor_msgs::Imu imu_msg;
	std_msgs::Float32 angle_msg;
    std_msgs::StringPtr lcd_msg(new std_msgs::String); 
    std_msgs::StringPtr lcd_msg_1(new std_msgs::String); 
    float a =0;
	rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();
    while (ros::ok())
    {
        if (imu->IMURead())
        {
            now = RTMath::currentUSecsSinceEpoch();
            sampleCount++;
            RTIMU_DATA imu_data = imu->getIMUData();

            imu_msg.header.stamp = ros::Time::now();
            imu_msg.header.frame_id = frame_id;

            imu_msg.orientation.x = imu_data.fusionQPose.x(); 
            imu_msg.orientation.y = imu_data.fusionQPose.y(); 
            imu_msg.orientation.z = imu_data.fusionQPose.z(); 
            imu_msg.orientation.w = imu_data.fusionQPose.scalar(); 

            imu_msg.angular_velocity.x = imu_data.gyro.x();
            imu_msg.angular_velocity.y = imu_data.gyro.y();
            imu_msg.angular_velocity.z = imu_data.gyro.z();

            imu_msg.linear_acceleration.x = imu_data.accel.x() * G_TO_MPSS;
            imu_msg.linear_acceleration.y = imu_data.accel.y() * G_TO_MPSS;
            imu_msg.linear_acceleration.z = imu_data.accel.z() * G_TO_MPSS;

            if ((now - displayTimer) > 5000) {
                angle_msg.data =(float)(imu_data.fusionPose.z() * RTMATH_RAD_TO_DEGREE+180);
                a = (float)(imu_data.fusionPose.z() * RTMATH_RAD_TO_DEGREE+180);
		angle_msg.data = roundf(angle_msg.data);
		if(angle_msg.data < toadodau) mmm = 360 - toadodau + angle_msg.data;
  		else mmm = angle_msg.data - toadodau;
  		if( mmm >180) mmm = mmm - 360;
		angle_msg.data = mmm;
                displayTimer = now;
		angle_pub.publish(angle_msg);
		string t1;
		ostringstream convert;
		convert << mmm;
		t1 = convert.str();
		lcd_msg_1->data = t1;
		lcd_pub_1.publish(lcd_msg_1);
		//cout << a << endl;
            }
            if ((now - rateTimer) > 10000) {
                sampleRate = sampleCount;
                sampleCount = 0;
                rateTimer = now;
                string t;
                ostringstream convert;
                a = roundf(a);
                convert << a;
                t = convert.str();
                //cout << t << endl;
                lcd_msg->data = t;
                /*
                int dem = 0; string st;
                if ( a == 0) lcd_msg->data = "0";

                else while ( a > 0){
                    int tam = a % 10;
                    a = a / 10;
                    st[dem] = char (tam) + 48;
                    dem++;

                }
                int k=0;
                for ( int i = st.length(); i>0 ; i--){
                    k=k*10;
                    k=int (st[i])-48;
                    cout<<k<<endl;
                }
                    */            
                imu_pub.publish(imu_msg);   			
		
                lcd_pub.publish(lcd_msg);
            }
        }
        ros::spinOnce();
        ros::Duration(imu->IMUGetPollInterval() / 1000.0).sleep();
    }
    return 0;
}
