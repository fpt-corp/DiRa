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

float CarControl::errorAngle(const Point &dst)
{
    if (dst.x == carPos.x) return 0;
    if (dst.y == carPos.y) return (dst.x < carPos.x ? -90 : 90);
    double pi = acos(-1.0);
    double dx = dst.x - carPos.x;
    double dy = carPos.y - dst.y; 
    if (dx < 0) return -atan(-dx / dy) * 180 / pi;
    return atan(dx / dy) * 180 / pi;
}

void CarControl::driverCar(const vector<Point> &left, const vector<Point> &right, float velocity)
{
   
    int i = left.size() - 8;
    float error = preError;
    Point pixels;
    
    if (DetectLane::phai == 0 && DetectLane::trai ==0)
    {
        pixels.x = ((right[i].x+left[i].x)/2 + (right[i+1].x+left[i+1].x)/2 + (right[i+2].x+left[i+2].x)/2)/3;
		pixels.y = (right[i+1].y );
		error = errorAngle(pixels);
    }
    if ( DetectLane::phai == 1 )
    {
        if (right[15].x == 230){
			error = 45;
		}
        else{
			pixels.x = (right[i].x + right[i+1].x + right[i+2].x)/3;
			pixels.y = right[i+1].y;
			error = errorAngle(pixels - Point(20, 0));
		}
    }
    if ( DetectLane::trai == 2 )
    {
        if (left[15].x == 10){
			error = -45;
		}
		else{
			pixels.x = (left[i].x + left[i+1].x + left[+2].x)/3;
			pixels.y = left[i+1].y;
			error = errorAngle(pixels + Point(5, 0));
		}
    }
    
    std_msgs::Float32 angle;
    std_msgs::Float32 camera_angle;

    camera_angle.data = camera;
    angle.data = error;
    
    pos_camera_publisher.publish(camera_angle);
    steer_publisher.publish(angle);
    
} 
