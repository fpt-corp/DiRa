#include "carcontrol.h"
#include "detectlane.h"

 
CarControl::CarControl()
{
    carPos = Point(ImageLine.cols/2,ImageLine.rows-1); 
    steer_publisher = node_obj1.advertise<std_msgs::Float32>("/set_steer_car_api",10);
    speed_publisher = node_obj2.advertise<std_msgs::Float32>("/set_speed_car_api",10);

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
   
    float angleControl = 0;
    double pi = acos(-1.0);
    double dx = dst.x - carPos.x;
    double dy = carPos.y - dst.y; 
    if (dst.x == carPos.x)
        angleControl = 0;
    else if (dst.y == carPos.y) 
        angleControl = (dst.x < carPos.x ? -50 : 50);
    else if (dx < 0) 
         angleControl = -atan(-dx / dy) * 180 / pi; 
    else angleControl = atan(dx / dy) * 180 / pi;

    if(angleControl*lastAngle < 0){
         angleControl = angleControl / 2;
    }
       
    //if(angleControl < 5) 
        //angleControl = 0;
    

    lastAngle = angleControl;
    //if(angleControl < -50) return -50;
    //else if (angleControl > 50) return 50;
    return angleControl;
}

void CarControl::driverCar(DetectLane &Detectlane, float velocity, int &carStatus)
{
 /*   float error =0;
    std_msgs::Float32 angle;
    std_msgs::Float32 speed;
    

    Point top = Point(Detectlane.midLine.get_X_by_Y(ImageLine.rows-20),ImageLine.rows-20);
    Point bot = Point(ImageLine.cols/2,ImageLine.rows-1);
    circle(ImageDraw, top, 1, Scalar(0, 255, 255), 5, 5, 0);
    circle(ImageDraw, bot, 1, Scalar(255, 255, 255), 5, 5, 0);
    imshow("DRAW", ImageDraw);
    // cout<<top<<endl<<bot<<endl;
    // line(ImageDraw,top, cv::Point(top.x, ImageLine.rows-1), cv::Scalar(255, 0, 0), 1);
    // line(ImageDraw,bot, cv::Point(bot.x, ImageLine.rows-1), cv::Scalar(0, 0, 255), 1);
    float steer = get_Angle(top,bot);

    cout<<"STEER: "<< steer<<endl;
    // if(abs(steer/2)<10){
    //      angle.data = 0;
    // }
    angle.data = steer;
    speed.data = velocity;
    steer_publisher.publish(angle);
    speed_publisher.publish(speed);  
        
 */
    float error = preError;

///////////////////////////////////////////
    Point targetPoint;
  //  targetPoint.y = 
    double param = 2*Detectlane.midLine.Function[0]*(ImageLine.rows-70)+Detectlane.midLine.Function[1];
    cout<<"HSG: "<<atan (param)*180/PI <<endl;
    
   
    Point top = Point(Detectlane.midLine.get_X_by_Y(ImageLine.rows-70),ImageLine.rows-70);
    Point bot = Point(ImageLine.cols/2,ImageLine.rows-1);
    circle(ImageDraw, top, 1, Scalar(0, 255, 255), 5, 5, 0);
    circle(ImageDraw, bot, 1, Scalar(255, 255, 255), 5, 5, 0);
    imshow("DRAW", ImageDraw);
    error = get_Angle(top,bot);

    cout << endl << " test error :"  << endl << "pre=" <<  preError << endl << " error =" << error << endl;
    waitKey(100);
///////////////////////////////////////////

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