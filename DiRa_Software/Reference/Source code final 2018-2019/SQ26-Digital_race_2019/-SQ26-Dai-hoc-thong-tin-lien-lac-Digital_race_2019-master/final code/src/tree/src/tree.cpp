#include "tree.h"

float bien_phai = 0;
int co = 0;
double tree::angle = 0;
int tree::start = 0;
bool led1 = false;
bool tree::set_offset = false;
double tree::offset = 28;

Mat  img_mau = imread("/home/nvidia/catkin_ws/src/tree/img/a.jpg"); // anh mau
//Mat rightSign = imread("/home/nvidia/Documents/img/phai.png", CV_LOAD_IMAGE_COLOR);


tree::tree(){
    tree_publisher = t.advertise<std_msgs::Float32>("set_tree",10);
    led_publisher = l.advertise<std_msgs::Bool>("led_status",1000);
    off_publisher = n.advertise<std_msgs::String>("lcd_print_3", 10);
    cvCreateTrackbar("LowH", "tree", &minThreshold[0], 179 );
    cvCreateTrackbar("HighH", "tree", &maxThreshold[0], 179);

    cvCreateTrackbar("LowS", "tree", &minThreshold[1], 255);
    cvCreateTrackbar("HighS", "tree", &maxThreshold[1], 255);

    cvCreateTrackbar("LowV", "tree", &minThreshold[2], 255);
    cvCreateTrackbar("HighV", "tree", &maxThreshold[2], 255);
}
tree::~tree(){}


float mapping_angle(float offset,float curr)
{
  return curr - offset;
}
void tree::tree_a(const Mat &src)
{

    //cout << offset << endl;
    float current_angle = tree::angle;
    float err_angle = mapping_angle(offset,current_angle) * 2;
    //bool nho = err_angle > 0 1:0; // phai 1 trai 0;
    
     Mat xanh, input;
    int co;
    vector<vector<Point> > contours;
	vector<vector<Point> > contours_xanh;
	vector<Vec4i> hierarchy;
	vector<Vec4i> hierarchy_xanh;
	vector<Rect> rect;
	vector<Rect> rect_xanh;
    cvtColor(src, input, CV_BGR2HSV);
    inRange(input,Scalar(minThreshold[0], minThreshold[1], minThreshold[2]),Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]),xanh);
    imshow("tree", xanh);

    int count =0;  
    for(int i=0; i <= 600; i++)
    {
        if(xanh.at<uchar>(420,i) != 0  ) count++;
    }
    cout << count << endl;
    if ( count < 5 ){
        co = 1; led1 = true;
    }
    else {
        co = 0; led1 = false;
    }
    cout <<"kiem tra"<< co << endl;
/*
    int erosion_size = 8;  
       
    findContours(xanh, contours_xanh, hierarchy_xanh,   CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    int max = 0;
    Rect finalR;
    if(contours_xanh.size() > 0)
		{
			for(int i = 0; i < contours_xanh.size(); ++i)
				{
					Rect r = boundingRect(contours_xanh.at(i));
                    Mat crop = xanh(r);

                    int count = countNonZero(crop);
                    if(count > max) 
                    {
                        max = count;
                        //cout << max << endl;
                        finalR = r;
                        if (max > 3000) {co = 1; led1 = true;}
                        else {co = 0; led1 = false;}
                        //cout << co << endl;
                    }
                }
            Mat crop = src(finalR);
            //imshow("detected", crop);
        }
    */
    // 
    if ( tree::angle > -20 && tree::angle < 20){
        std_msgs::Bool led;
        led.data = led1;
        led_publisher.publish(led); 
    }    
    std_msgs::Float32 d;
    if (co == 1 && tree::angle > -20 && tree::angle < 20){
	d.data = 0;
    }
    else d.data = 100; 
    tree_publisher.publish(d);

    std_msgs::StringPtr f(new std_msgs::String);
    string t1;
	ostringstream convert;
	convert << offset;
	t1 = convert.str();
	f->data = t1;
    
    off_publisher.publish(f);
}
