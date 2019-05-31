#include "carcontrol.h"
 
CarControl::CarControl()
{
    carPos.x = 120;
    carPos.y = 300;
    steer_publisher = node_obj1.advertise<std_msgs::Float32>("Team417_steerAngle",10);
    speed_publisher = node_obj2.advertise<std_msgs::Float32>("Team417_speed",10);

    t_kP = 500;
    t_kI = 1;
    t_kD = 1600;

    createTrackbar("P", "PID", &t_kP, 5000);
    createTrackbar("I", "PID", &t_kI, 5000);
    createTrackbar("D", "PID", &t_kD, 5000);
}

CarControl::~CarControl() {}

float CarControl::errorAngle(const Point &dst)
{
    if (dst.x == carPos.x) return 0;
    if (dst.y == carPos.y) return (dst.x < carPos.x ? -90 : 90);
    float pi = acos(-1.0);
    float dx = dst.x - carPos.x;
    float dy = carPos.y - dst.y;
    if (dx < 0) return -atan(-dx / dy) * 180 / pi;
    return atan(dx / dy) * 180 / pi;
}

void CarControl::driverCar(const vector<Point> &left, const vector<Point> &right, float velocity)
{
    int i = left.size() - 11;
    float error = preError;
    while (left[i] == LaneDetect::null && right[i] == LaneDetect::null) {
        i--;
        if (i < 0) return;
    }
    if (left[i] != LaneDetect::null && right[i] !=  LaneDetect::null)
    {
        error = errorAngle((left[i] + right[i]) / 2);
    } 
    else if (left[i] != LaneDetect::null)
    {
        error = errorAngle(left[i] + Point(laneWidth / 2, 0));
    }
    else
    {
        error = errorAngle(right[i] - Point(laneWidth / 2, 0));
    }

    std_msgs::Float32 angle;
    std_msgs::Float32 speed;

    kP = (float)t_kP / 1000;
    kI = (float)t_kI / 1000;
    kD = (float)t_kD / 1000;

    p_error = error;
    i_error += error;
    d_error = error - preError;

    float _angle = kP * p_error + kD * d_error + kI * i_error;

    angle.data = _angle;
    speed.data = velocity;

    preError = error;

    steer_publisher.publish(angle);
    speed_publisher.publish(speed);
} 

void CarControl::turnLeft(vector<Point> LeftLane,float velocity)
{
    cerr<<"TURN LEFT"<<endl;
    vector<Point> Pnull;
    for(int j = 0; j < 32; ++j) Pnull.push_back(LaneDetect::null);
    driverCar(LeftLane,Pnull, velocity);
}

void CarControl::turnRight(vector<Point> RightLane, float velocity)
{
    cerr<<"TURN RIGHT"<<endl;
    vector<Point> Pnull;
    for(int j = 0; j < 32; ++j) Pnull.push_back(LaneDetect::null);
    driverCar(Pnull,RightLane, velocity);
}

void CarControl::moveLeftSide(vector<Point> LeftLane,float velocity)
{
    vector<Point> Pnull;
    for(int j = 0; j < 32; ++j) Pnull.push_back(LaneDetect::null);
    float tmpLaneWidth = laneWidth;
    laneWidth *= 0.9;
    driverCar(LeftLane, Pnull, velocity);
    laneWidth = tmpLaneWidth;
}

void CarControl::moveRightSide(vector<Point> RightLane, float velocity)
{
    vector<Point> Pnull;
    for(int j = 0; j < 32; ++j) Pnull.push_back(LaneDetect::null);
    float tmpLaneWidth = laneWidth;
    laneWidth *= 0.9;
    driverCar(Pnull,RightLane, velocity);
    laneWidth = tmpLaneWidth;
}
