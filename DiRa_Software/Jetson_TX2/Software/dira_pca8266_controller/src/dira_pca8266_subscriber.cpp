#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "ackermann_msgs/AckermannDriveStamped.h"
#include "DiRa_Traxxas_Controller.h"

PCA9685 *pca9685 = new PCA9685() ;

// Open Source Ackermann Dirve compatible for keyboard
void process_ackermann_msgs(const ackermann_msgs::AckermannDriveStamped &msg)
{
    double speed = msg.drive.speed * 75;
    if (speed != 0)
    {
        api_set_FORWARD_control(pca9685, speed);
    }
    else
    {
        api_set_BRAKE_control(pca9685, speed);
    }
	ROS_INFO("new throttle: [%f]",speed);

    double steer_angle=msg.drive.steering_angle * 300;
    api_set_STEERING_control(pca9685, steer_angle);

}
// Open Source Ackermann Drive compatible for joystick
void process_ackermann_msgs_j(const ackermann_msgs::AckermannDrive &msg)
{
    double speed = msg.speed * 75;
    if (speed != 0)
    {
        api_set_FORWARD_control(pca9685, speed);
    }
    else
    {
        api_set_BRAKE_control(pca9685, speed);
    }
	ROS_INFO("new throttle: [%f]",speed);

    double steer_angle=msg.steering_angle * 300;
    api_set_STEERING_control(pca9685, steer_angle);

}

// Legacy version of Traxxas Controller
void set_speed_car(const std_msgs::Float32::ConstPtr& msg)
{
    double speed = msg->data;
    if (speed != 0)
    {
        api_set_FORWARD_control(pca9685, speed);
    }
    else
    {
        api_set_BRAKE_control(pca9685, speed);
    }
	ROS_INFO("new throttle: [%f]",speed);
}
void set_steer_car(const std_msgs::Float32::ConstPtr& msg)
{
    double steer_angle=msg->data;
    api_set_STEERING_control(pca9685, steer_angle);
}

void set_pos_camera(const std_msgs::Float32::ConstPtr& msg)
{
    double camera_angle = msg->data;
    api_set_CAMERA_control(pca9685, camera_angle);
}
// For Diffirential Drive motor only
void set_left_motor(const std_msgs::Float32::ConstPtr& msg)
{
    double speed = msg->data;
    api_set_left_motor(pca9685, speed);
}
void set_right_motor(const std_msgs::Float32::ConstPtr& msg)
{
    double speed = msg->data;
    api_set_right_motor(pca9685, speed);
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "DiRa_PCA9685_Controller");
    api_pwm_pca9685_init( pca9685 );
    double spd = 0;
    api_set_FORWARD_control(pca9685, spd);
    ros::NodeHandle n;
    ros::Subscriber key = n.subscribe("/ackermann_cmd", 10, process_ackermann_msgs);
    ros::Subscriber joy = n.subscribe("/ackermann_cmd_j", 10, process_ackermann_msgs_j);

    ros::Subscriber left = n.subscribe("/left_motor", 10, set_left_motor);
    ros::Subscriber right = n.subscribe("/right_motor", 10, set_right_motor);

    ros::Subscriber sub1 = n.subscribe("/set_speed_car_api", 10, set_speed_car);
    ros::Subscriber sub2 = n.subscribe("/set_steer_car_api", 10, set_steer_car);
    ros::Subscriber sub3 = n.subscribe("/set_pos_camera_api", 10, set_pos_camera);			
    ros::spin();
    return 0;
}

