#include "carcontrol.h"
#include "detectlane.h"
int dem=0;
CarControl::CarControl()
{
    carPos.x = 320;
    carPos.y = 480;
    steer_publisher = node_obj1.advertise<std_msgs::Float32>("/set_steer_car_api",1);
    speed_publisher = node_obj2.advertise<std_msgs::Float32>("/set_speed_car_api",1);
    pub = n.advertise<std_msgs::String>("/lcd_print", 30);

    t_kP = 100; //100
    t_kI = 50;//50
    t_kD = 10;//10
    dt = 0.1;

    createTrackbar("P", "PID", &t_kP, 5000);
    createTrackbar("I", "PID", &t_kI, 5000);
    createTrackbar("D", "PID", &t_kD, 5000);
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

void CarControl::driverCar(const Mat &view,const vector<Point> left, const vector<Point> right , float velocity,int flag, bool sensor)
{

    dem++;
    Mat viewPoint = view.clone();
    std_msgs::Float32 angle;
    std_msgs::Float32 speed;
    std_msgs::String str;
    str.data = "0:0: speed " + to_string(velocity);
    if(dem==30){
	pub.publish(str);
	dem=0;
    }

    float error;
    int xTam = 0, yTam = 0;
    if(!sensor) maxVelocity = 0 ;
    else maxVelocity = velocity;
    
    int nLeft = 0, nRight = 0;
    nLeft = left.size();
    nRight = right.size();
    int xLeft = 0, xRight = 0, yLeft = 0, yRight = 0;
	
    //Truong hop khong co bien bao
    if(flag != 1 && flag != 2)
    {
        //Truong hop co 2 lane
        if(nLeft > 0 && nRight > 0)
            {
                for(int i = 0; i < nLeft; i++)
                    {
                        xLeft += left[i].x;
                        yLeft += left[i].y;
                    }
                for(int i = 0; i < nRight; i++)
                    {
                        xRight += right[i].x;
                        yRight += right[i].y;
                    }
                xTam = (xLeft/nLeft + xRight/nRight) / 2;
                yTam = (yLeft/nLeft + yRight/nRight) / 2;
		yTam += h/2;
            }
        else if(nLeft > 0) //Truong hop chi co lane trai
            {
                for(int i = 0; i < nLeft; i++)
                    {
                        xLeft += left[i].x;
                        yLeft += left[i].y;
                    }
                xTam = xLeft/nLeft + bam_lane;
		if(xTam > w) xTam -= bam_lane;
                yTam = yLeft/nLeft; 
		yTam += h/2;                         
            }
        else if(nRight > 0) //Truong hop chi co lane phai
            {
                for(int i = 0; i < nRight; i++)
                    {
                        xRight += right[i].x;
                        yRight += right[i].y;
                    }
                xTam = xRight/nRight - bam_lane;
		if(xTam < 0) xTam += bam_lane;
                yTam = yRight/nRight;
		yTam += h/2;        
            }
        else //Truong hop mat 2 lane
            {
                xTam = w/2;
                
            } 
    }
    else if(flag == 1) //Truong hop nhan bien bao re trai
    {
        if(nLeft > 0) //Truong hop re trai ma co lane trai
            {
                for(int i = 0; i < nLeft; i++)
                    {
                        xLeft += left[i].x;
                        yLeft += left[i].y; 
                    }
                    xTam = xLeft/nLeft + khoang_cach_trai;
		    if(xTam > w ) xTam -= khoang_cach_trai ;
                    yTam = yLeft/nLeft;
		    yTam += h/2;
            }
        else //Truong hop re trai ma khong co lane trai
            {

		if(nRight > 0)
			{
				for(int i = 0; i < nRight; i++)
					{
						xRight += right[i].x;
						yRight += right[i].y;
					}
				xTam = xRight/nRight - khoang_cach_phai*2;
				if(xTam < 0) xTam += khoang_cach_phai*2;
				yTam = yRight/nRight;
				yTam += h/2;
				
					
			}
		else
			{
                		xTam = w/2 - khoang_cach_trai;
               										
			}             
            }
    }
    else if(flag == 2) //Truong hop bien bao re phai 
    {
        if(nRight > 0) //Truong hop re phai co lane phai
            {
                for(int i = 0; i < nRight; i++)
                {
                    xRight += right[i].x;
                    yRight += right[i].y;
                }
                
                xTam = xRight/nRight  - khoang_cach_phai;
		if(xTam < 0) xTam += khoang_cach_phai ;
                yTam = yRight/nRight;
		yTam += h/2;
            
            }
        else //Truong hop re phai khong co lane phai
            {
		if(nLeft > 0)
			{
				for(int i = 0; i < nLeft; i++)
					{
						xLeft += left[i].x;
						yLeft += left[i].y;
					}
				xTam = xLeft/nLeft + khoang_cach_phai*2;
				if(xTam > w) xTam += khoang_cach_phai*2;
				yTam = yLeft/nLeft;
				yTam += h/2;
				
					
			}
		else
			{
                		xTam = w/2 + khoang_cach_phai;
               										
			}               
            }
    }
    
    
    
    error = errorAngle(Point(xTam,h/2));
    if(xTam > w ) xTam = w;
    if(xTam < 0 ) xTam = 0;
    circle(viewPoint, Point(xTam, h/2), 3, Scalar(0,255,255), 3);
    line(viewPoint, carPos, Point(xTam,h/2), Scalar(0,255,0), 3);
    imshow("Direct",viewPoint);
    
	//PID
    kP = (float)t_kP / 1000;
    kI = (float)t_kI / 1000;
    kD = (float)t_kD / 1000;
   
    p_error = error;
    i_error += error * dt;
    d_error = (error - preError)/dt;
    
     
    
 
    
    float _angle = kP * p_error + kD * d_error + kI * i_error;
    
    //if(_angle > MAX_STEERING ) _angle = MAX_STEERING;
    //if(_angle < MIN_STEERING ) _angle = MIN_STEERING;
    //cout << "Error and angle: " << error << " " << _angle << endl;
    if(error > MAX_STEERING ) error = MAX_STEERING;
    if(error < MIN_STEERING ) error = MIN_STEERING; 
    if(velocity > 0) velocity = maxVelocity - (fabs(error)/maxAngle * maxVelocity);
    if(velocity < 10 && maxVelocity != 0) velocity = 10;
    cout << "Error and Velocity: " <<error << "\t"<< velocity << endl;
    angle.data = error;
    speed.data = velocity;

    preError = error;

    steer_publisher.publish(angle);
    speed_publisher.publish(speed);
    
} 
