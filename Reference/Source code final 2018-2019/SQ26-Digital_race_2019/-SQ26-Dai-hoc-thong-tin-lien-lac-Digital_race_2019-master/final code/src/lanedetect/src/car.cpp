#include "car.h"

double camera = 0;


CarControl::CarControl()
{
    carPos.x = 240;
    carPos.y = 560;
    //steer_publisher = node_obj1.advertise<std_msgs::Float32>("set_steer_car_api",10);
    pos_camera_publisher = node_obj2.advertise<std_msgs::Float32>("set_pos_camera_api",10);
    steer_publisher = node_obj1.advertise<std_msgs::Float32>("set_error",10);
}

CarControl::~CarControl() {}



void CarControl::driverCar(const vector<Point> &left, const vector<Point> &right)
{
   
    float error = 0;
    Point pixels;
   
    
    if ( DetectLane::phai == 0 && DetectLane::trai ==0)
    {
        error = camera = 0;
    }

    if ( DetectLane::trai == 1)
    {
        camera = 10; //5
        //error = DetectLane::bien_trai;
	error = 135 - DetectLane::bien_trai;
       
    }

    if (DetectLane::phai == 1)
    {
        camera = -15; //-20
        //error = DetectLane::bien_phai;
        error = 500 - DetectLane::bien_phai;
	//cout << DetectLane::bien_phai << endl;
    }
    
	//cout << error<<endl;
    
    std_msgs::Float32 angle;
    std_msgs::Float32 camera_angle;

    camera_angle.data = camera;
    angle.data = error;
    
    pos_camera_publisher.publish(camera_angle);
    steer_publisher.publish(angle);
    
} 
