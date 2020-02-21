#include "iostream"
#include "ros/ros.h"
#include "std_msgs/Float32.h"
//#include "ackermann_msgs/AckermannDriveStamped.h"
#include "dira_pca9685_controller.h"

PCA9685 *pca9685 = new PCA9685() ;
/*
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
*/
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
int main(int argc, char **argv)
{
    ros::init(argc, argv, "DiRa_PCA9685_Controller");
    ros::NodeHandle n;
    ros::NodeHandle nh("~");
    nh.param("pwm_pca9685", PWM_FREQ, 100);

    api_pwm_pca9685_init( pca9685 );
    double spd = 0;
    api_set_FORWARD_control(pca9685, spd);
    
    ros::Rate r(1);
    
    
    /* Ackermann Drive with joystick
    ros::Subscriber key = n.subscribe("/ackermann_cmd", 10, process_ackermann_msgs);
    ros::Subscriber joy = n.subscribe("/ackermann_cmd_j", 10, process_ackermann_msgs_j);
    */

    /* The car's speed control
       Speed: Min: -100 and Max: 100
       If the car is running and recieve 0. The car will break and stop.
       If the car is running and recieve negative value (if high enough). The car will break and start running in the opostite direction.
    */
    ros::Subscriber sub1 = n.subscribe("/set_speed", 10, set_speed_car);

    /* The car's turning angle control
       Turn angle: Min -60 and Max 60
       The degree the car will turn.
       0 degree - The car will run straight.
    */
    ros::Subscriber sub2 = n.subscribe("/set_angle", 10, set_steer_car);	
    ros::spin();
    return 0;
}

