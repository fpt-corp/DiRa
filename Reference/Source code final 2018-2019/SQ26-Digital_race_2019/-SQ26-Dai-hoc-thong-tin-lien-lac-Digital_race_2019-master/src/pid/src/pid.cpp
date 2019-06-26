#include "pid.h"

float pid::error = 0;

float pid::speed = 0;
int pid::a=0;
int pid::start = 0;
float P,D,I = 0;
float previous_error = 0;
int count = 0;
pid::pid()
{
    steer_publisher = aa.advertise<std_msgs::Float32>("set_steer_car_api",10);
    lcd_publisher = lcd.advertise<std_msgs::String>("lcd_print_1", 10);
    speed_publisher = bb.advertise<std_msgs::Float32>("set_speed",10);
        
}

pid::~pid() {}

void pid::pid_control()
{
	float kP = 0.5, kD = 0.01, kI=0.0001 ;
	if ( pid::a == 2 ) kP = 0.45;	
	if ( pid::a == 3 ) kP = 0.45;
    int max_e = 50;
	float err = 0;
    P = pid::error;
    I = I + pid::error;
    D = pid::error - previous_error;
	
    float PID_value = (kP*P) + (kI*I) + (kD*D);
    previous_error = pid::error;
    if (I < -max_e) I = -max_e;
    if (I > max_e) I= max_e;

	if ( pid::a == 1 )
	{
		pid::speed = 1.6;
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;

		if (PID_value >= 0) err = PID_value;
		else err = -PID_value;

		pid::speed = pid::speed - (err/200);	
	}
	if ( pid::a == 2){
		pid::speed = 1.8;
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;

		if (PID_value >= 0) err = PID_value;
		else err = -PID_value;

		pid::speed = pid::speed - (err/120);
	}
	if( pid::a == 3 ){
		pid::speed = 2;
		if (PID_value < -70) PID_value = -70;
		if (PID_value > 70) PID_value =   70;

		if (PID_value >= 0) err = PID_value;
		else err = -PID_value;

		pid::speed = pid::speed - (err/100);
		
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;
	}
	if ( pid::a == 4 )
	{
		pid::speed = 1;
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;	
	}
	if ( pid::a == 5 )
	{
		pid::speed = 1.4;
		if (PID_value < -60) PID_value = -60;
		if (PID_value > 60) PID_value =   60;	
	}	
	if (pid:: a == 0) {pid::speed = 0; PID_value = 0; }
	
	
    //cout<<PID_value<<endl;
    std_msgs::StringPtr msg_lcd(new std_msgs::String); 
    if ( pid::start != 0 && pid::a == 0){ 
	msg_lcd->data = "ok  ";
	lcd_publisher.publish(msg_lcd);
    }
    if ( pid::a != 0){ 
	msg_lcd->data = "run ";
	lcd_publisher.publish(msg_lcd);
    }
    if (pid::start ==0 && pid::a == 0) { msg_lcd-> data = "stop";
    	lcd_publisher.publish(msg_lcd);
    }

    std_msgs::Float32 er; 
    er.data = PID_value; 
    steer_publisher.publish(er);

    std_msgs::Float32 sp;
    sp.data = pid::speed; 
    speed_publisher.publish(sp);
    
	//cout<<"error "<< error <<endl;
}

