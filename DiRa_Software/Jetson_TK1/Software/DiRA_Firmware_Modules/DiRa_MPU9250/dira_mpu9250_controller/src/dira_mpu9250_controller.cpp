#include <RTIMULib.h>
#include <ros/ros.h>
#include <ros/package.h>
#include <sensor_msgs/Imu.h>
#include "std_msgs/Float32.h"
static const double G_TO_MPSS = 9.80665;

int main(int argc, char **argv)
{
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;
    ros::init(argc, argv, "rtimulib_node");
    ROS_INFO("Imu driver is now running");
    ros::NodeHandle nh;
	ros::NodeHandle nh2;
    std::string calibration_file_path;
    
    if(!nh.getParam("/mpu_9250/calibration_file_name", calibration_file_path))
    {
        ROS_ERROR("The calibration_file_path parameter must be set to use a "
                  "calibration file.");
        ROS_BREAK();
    }

    std::string calibration_file_name = "RTIMULib";
    if(!nh.getParam("/mpu_9250/calibration_file_name", calibration_file_name))
    {
        ROS_WARN_STREAM("No calibration_file_name provided - default: "
                        << calibration_file_name);
    }

    std::string frame_id = "imu_link";
    if(!nh.getParam("/mpu_9250/frame_id", frame_id))
    {
        ROS_WARN_STREAM("No frame_id provided - default: " << frame_id);
    }

    ros::Publisher imu_pub = nh.advertise<sensor_msgs::Imu>("imu", 1);
	ros::Publisher angle_pub = nh2.advertise<std_msgs::Float32>("imu_angle", 1);
    // Load the RTIMULib.ini config file
    RTIMUSettings *settings = new RTIMUSettings(calibration_file_path.c_str());

    RTIMU *imu = RTIMU::createIMU(settings);

    if ((imu == NULL))
    {
        ROS_ERROR("No Imu found");
        ROS_BREAK();
    }

    // Initialise the imu object
    imu->IMUInit();

    // Set the Fusion coefficient
    //imu->setSlerpPower(0.02);
    // Enable the sensors
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    sensor_msgs::Imu imu_msg;
	std_msgs::Float32 angle_msg;
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
                angle_msg.data =(float)(imu_data.fusionPose.z() * RTMATH_RAD_TO_DEGREE+180.0);
                displayTimer = now;
            }
            if ((now - rateTimer) > 1000000) {
                sampleRate = sampleCount;
                sampleCount = 0;
                rateTimer = now;
                imu_pub.publish(imu_msg);   			
			    angle_pub.publish(angle_msg);
            }
        }
        ros::spinOnce();
        ros::Duration(imu->IMUGetPollInterval() / 1000.0).sleep();
    }
    return 0;
}
