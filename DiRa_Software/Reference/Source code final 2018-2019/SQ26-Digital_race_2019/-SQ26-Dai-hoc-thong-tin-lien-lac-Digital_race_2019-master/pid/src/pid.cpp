#include "pid.h"

float pid::error = 0;
float pid::speed = 0;
int pid::a=0;
float P,D,I = 0;
float previous_error = 0;
pid::pid()
{
    steer_publisher = aa.advertise<std_msgs::Float32>("set_steer_car_api",10);

    speed_publisher = bb.advertise<std_msgs::Float32>("set_speed",10);
        
}

pid::~pid() {}

void pid::pid_control()
{
	float kP = 0.9, kD = 0.01, kI=0.0001 ;
    int max_e = 50;
	
    P = pid::error;
    I = I + pid::error;
    D = pid::error - previous_error;
	
    float PID_value = (kP*P) + (kI*I) + (kD*D);
    previous_error = pid::error;
    if (I < -max_e) I = -max_e;
    if (I > max_e) I= max_e;

	if ( pid::a == 2 )
	{
		pid::speed = 1.2;
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;	
	}
	if ( pid::a == 1){
		if (-5 <= PID_value && PID_value<= 5) { PID_value = 0; pid::speed = 1.6; }
		if (5 < PID_value && PID_value <= 15) { PID_value = 10; pid::speed = 1.5; }
		if (15 < PID_value && PID_value <= 25) { PID_value = 20; pid::speed = 1.5;}
		if (25 < PID_value && PID_value <= 35) { PID_value = 30; pid::speed = 1.4;}
		if (35 < PID_value && PID_value <= 45) { PID_value = 40; pid::speed = 1.4;}
		if (45 < PID_value && PID_value <= 55) { PID_value = 50; pid::speed = 1.3;}
		if (PID_value > 55) { PID_value = 60; pid::speed = 1.2;}
		
		if (-15 <= PID_value && PID_value < -5) { PID_value = -10; pid::speed = 1.5;}
		if (-25 <= PID_value && PID_value < -15) { PID_value = -20; pid::speed = 1.5;}
		if (-35 <= PID_value && PID_value < -25) { PID_value = -30; pid::speed = 1.4;}
		if (-45 <= PID_value && PID_value < -35) { PID_value = -40; pid::speed = 1.4;}
		if (-55 <= PID_value && PID_value < -45) { PID_value = -50; pid::speed = 1.3;}
		if (PID_value < -55) { PID_value = -60; pid::speed = 1.2;}
	 	
	}
		
	if (pid:: a == 0) {pid::speed = 0; PID_value = 0; }
    cout<<PID_value<<endl;
    std_msgs::Float32 er; 
    er.data = PID_value; 
    steer_publisher.publish(er);

    std_msgs::Float32 sp;
    sp.data = pid::speed; 
    speed_publisher.publish(sp);
    
	//cout<<"error "<< error <<endl;
}

