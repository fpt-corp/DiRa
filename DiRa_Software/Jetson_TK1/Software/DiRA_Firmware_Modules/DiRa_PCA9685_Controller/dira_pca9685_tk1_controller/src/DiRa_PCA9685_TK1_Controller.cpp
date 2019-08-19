
#include "DiRa_PCA9685_TK1_Controller.h"
#include "ros/ros.h"
#include "math.h"

int direction;
int map (double x, int in_min, int in_max, int out_min, int out_max)
{
    double toReturn =  1.0 * (x - in_min) * (out_max - out_min) /
            (in_max - in_min) + out_min ;
    return (int)(round(toReturn));
}

void api_pwm_pca9685_init( PCA9685 *pca9685)
{
    // Initialize the PWM board
    int err = pca9685->openPCA9685();
    if (err < 0)
    {
        std::cout<< std::endl<< "Error: %d"<< pca9685->error<< std::flush;
    }

    std::cout<< std::endl<< "PCA9685 Device Address: 0x"<< std::hex
        << pca9685->kI2CAddress<< std::dec<< std::endl;

    pca9685->setAllPWM(0,0) ;
    pca9685->reset() ;
    pca9685->setPWMFrequency(PWM_FREQ) ;
    // Set the PWM to "neutral" (1.5ms)
    sleep(1) ;
    direction = 0;
    int pwm_steer_middle = map( 0, MIN_ANGLE, MAX_ANGLE, STEERING_MAX_RIGHT, STEERING_MAX_LEFT );
    int pwm_camera_middle = map( 0, MIN_SERVO, MAX_SERVO, CAMERA_MAX_RIGHT, CAMERA_MAX_LEFT );
    ROS_INFO("dir value:%d",direction);
    pca9685->setPWM(CAMERA_CHANNEL, 0, pwm_camera_middle);
    pca9685->setPWM(STEERING_CHANNEL, 0, pwm_steer_middle);
    pca9685->setPWM(THROTTLE_CHANNEL, 0, THROTTLE_NEUTRAL);
    sleep(1) ;
}

int api_set_FORWARD_control( PCA9685 *pca9685,double &throttle_val)
{       
    if(throttle_val > 0)
    {
	if(direction == -1 && throttle_val > 20)
	{
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_FORWARD);
		ROS_INFO("dir value:%d",direction);
        	usleep(187500);
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
		direction = 0;
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
				
	}
	else if(direction == -1 && throttle_val <=20)
	{
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_FORWARD);
		ROS_INFO("dir value:%d",direction);
        	usleep(187500);
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
		direction = 0;
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
		int pwm = map(15, 0, 100, THROTTLE_NEUTRAL, THROTTLE_MAX_FORWARD );
		pca9685->setPWM(THROTTLE_CHANNEL,0, pwm);
		usleep(187500);
		
	}
        direction = 1;
        int pwm = map( throttle_val, 0, 100, THROTTLE_NEUTRAL, THROTTLE_MAX_FORWARD );
	if(pwm > THROTTLE_MAX_FORWARD)
	{
		pwm = THROTTLE_MAX_FORWARD;
	}
	
        pca9685->setPWM(THROTTLE_CHANNEL,0, pwm);
	ROS_INFO("forward dir value:%d pwm value:%d",direction, pwm);
    }
    else if(throttle_val < 0)
    {
	if(direction == 1 && throttle_val < -20)
	{
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_REVERSE);
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
		direction = 0;
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
	}
	else if(direction == 1 && throttle_val >= -20)
	{
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_REVERSE);
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
		pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
		direction = 0;
		ROS_INFO("dir value:%d",direction);
		usleep(187500);
		int pwm = 4096 - map( abs(-20), 0, 100 , 4096 - THROTTLE_NEUTRAL , 4096 - THROTTLE_MAX_REVERSE);
		pca9685->setPWM(THROTTLE_CHANNEL,0, pwm);
		usleep(187500);
	}
        direction = -1;
        int pwm = 4096 - map( abs(throttle_val), 0, 100 , 4096 - THROTTLE_NEUTRAL , 4096 - THROTTLE_MAX_REVERSE);
	if(pwm < THROTTLE_MAX_REVERSE)
	{
		pwm = THROTTLE_MAX_REVERSE;
	}
        pca9685->setPWM(THROTTLE_CHANNEL,0, pwm);
	ROS_INFO("revese dir value:%d pwm value:%d",direction, pwm);
    }
}

int api_set_BRAKE_control( PCA9685 *pca9685,double &throttle_val)
{
    if(direction == 0)
    {
        pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
	ROS_INFO("dir value:%d",direction);
        usleep(1000);
	direction == 0;
    }
    else if(direction == 1)
    {
        pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_REVERSE);
	ROS_INFO("dir value:%d",direction);
        usleep(1000);
	pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
	ROS_INFO("dir value:%d",direction);
	direction = 0;
    }
    else if(direction == -1)
    {
        pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_MAX_FORWARD);
	ROS_INFO("dir value:%d",direction);
        usleep(1000);
	pca9685->setPWM(THROTTLE_CHANNEL,0, THROTTLE_NEUTRAL);
	ROS_INFO("dir value:%d",direction);
	direction = 0;
    }
}

int api_set_STEERING_control( PCA9685 *pca9685,double &theta)
{
    if( theta < MIN_ANGLE)
        theta = MIN_ANGLE;

    if( theta > MAX_ANGLE )
        theta = MAX_ANGLE;
        
	int pwm1 = map( -theta, MIN_ANGLE, MAX_ANGLE, STEERING_MAX_RIGHT, STEERING_MAX_LEFT); 
    pca9685->setPWM(STEERING_CHANNEL,0, pwm1);
    return pwm1;
}
int api_set_CAMERA_control( PCA9685 *pca9685,double &theta)
{
    if( theta < MIN_SERVO)
        theta = MIN_SERVO;

    if( theta > MAX_SERVO )
        theta = MIN_SERVO;
        
    int pwm2 = map( theta, MIN_SERVO, MAX_SERVO, CAMERA_MAX_RIGHT, CAMERA_MAX_LEFT ); 
    pca9685->setPWM(CAMERA_CHANNEL,0, pwm2);
    return pwm2;
}

