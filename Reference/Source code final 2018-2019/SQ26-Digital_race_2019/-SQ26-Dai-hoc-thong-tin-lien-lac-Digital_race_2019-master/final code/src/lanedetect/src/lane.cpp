#include "lane.h"



int min(int a, int b)
{
    return a < b ? a : b;
}

int DetectLane::slideThickness = 10;
int DetectLane::BIRDVIEW_WIDTH = 240;
int DetectLane::BIRDVIEW_HEIGHT = 320;
int DetectLane::VERTICAL = 0;
int DetectLane::HORIZONTAL = 1;
int DetectLane::phai= 0;
int DetectLane::trai= 0;
int DetectLane::bien_trai=0;
int DetectLane::bien_phai=0;
int DetectLane::imgCount =0;
int DetectLane::imgCount1 =0;
int DetectLane::iscomplete = 0;
int DetectLane::lane = 0;
int hcn = 0;
Point DetectLane::null = Point();
  Mat img_hsv;
  Mat src_clone1;
DetectLane::DetectLane() {
    cvCreateTrackbar("LowH", "Threshold", &minThreshold[0], 179 );
    cvCreateTrackbar("HighH", "Threshold", &maxThreshold[0], 179);

    cvCreateTrackbar("LowS", "Threshold", &minThreshold[1], 255);
    cvCreateTrackbar("HighS", "Threshold", &maxThreshold[1], 255);

    cvCreateTrackbar("LowV", "Threshold", &minThreshold[2], 255);
    cvCreateTrackbar("HighV", "Threshold", &maxThreshold[2], 255);

    cvCreateTrackbar("depth", "Threshold", &ThresholdDepth[0], 255);
}

DetectLane::~DetectLane(){}

vector<Point> DetectLane::getLeftLane()
{
    return leftLane;
}

vector<Point> DetectLane::getRightLane()
{
    return rightLane;
}

void mouse_callback(int  event, int  x, int  y, int  flag, void *param)
{
    Mat &img_hsv = *((Mat*)param);
    if (event == EVENT_MOUSEMOVE) {
        Vec3b val = img_hsv.at<Vec3b>(y,x);
        cout << "x= " << x << " y= " << y << "val= "<<val<< endl;
    }
}

void DetectLane::update(const Mat &src)
{
    Mat img = preProcess(src);
    ss = src.clone();
    //update2();
    Mat src_clone = src.clone(); 
    cvtColor(src_clone,img_hsv,CV_BGR2HSV);
    
    //setMouseCallback("HSV",mouse_callback,&img_hsv);
   // imshow("HSV", img_hsv);
    leftLane.clear();
    rightLane.clear();
    //Mat lane;
    //lane = Mat::zeros(img.size(), CV_8UC3);
    int nline = 1;
    /*for (int ilr = 0; ilr < 5; ilr++)
    {
        leftLane.push_back(null);
        rightLane.push_back(null);
    }*/
    //for (int j = 435; j< 476; j = j + 20){
        // bien trai
        if ( trai == 1 ){
            for (int i = 400; i>= 1; i--)
            {
                if(img.at<uchar>(420,i) == 255)
                {
                    bien_trai = i;
                   // cout<<"bien_trai: "<< bien_trai <<endl;
                    i = 0;
                }
            } 
        }
        // bien phai
        if ( phai == 1){
            for (int i = 300; i < 640; i++)
            {
                if(img.at<uchar>(420,i) == 255)
                {
		    bien_phai = i;
                    //cout<<"bien_phai: "<<bien_phai<<endl;
                    i = 700;
                }
            }
        }

        //leftLane[nline-1] = Point2i( bien_trai , j);
		//rightLane[nline-1] = Point2i( bien_phai , j); 
        //nline +=1;
    //}
    /*for (int i = 0; i < leftLane.size(); i++)
    {
        if (leftLane[i] != null)
        {
            circle(lane, leftLane[i], 1, Scalar(0,0,255), 2, 8, 0 );
        }
    }
    imshow("Lane Detect", lane);
    */
    imshow("Threshold", img);
    

   // imshow("view", src_clone);
    //imshow("c", xanh);
}
/*
void DetectLane::update2(const Mat &src)
{ if(DetectLane::iscomplete == 1){
    Mat gray = ss.clone();
    cvtColor(gray,gray,CV_BGR2GRAY); 
    Mat depth;
    threshold( src, depth, 1, 65535,0);
    depth.convertTo(depth, CV_8U);
    bitwise_and(gray,depth,gray);
    threshold( gray, gray, ThresholdDepth[0], 255,0);
    //GaussianBlur( gray, gray, Size( 5, 5 ), 0, 0 );
    erode(gray, gray, Mat(), Point(-1, -1), 3, 1, 1);
    dilate(gray, gray, Mat(), Point(-1, -1), 8, 1, 1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<Rect> rect;
    findContours(gray, contours, hierarchy,   CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    int max = 0;
    Rect finalR;
    if(contours.size() > 0)
		{
			for(int i = 0; i < contours.size(); ++i)
				{
					Rect r = boundingRect(contours.at(i));
                    Mat crop = gray(r);

                    int count = countNonZero(crop);
                    if(count > max) 
                    {
                        max = count;
                        finalR = r;
                    }
                }
        rectangle(gray, finalR, Scalar(255, 255, 255), -1);
        imshow("s", gray);
        
        }
}
}
*/

Mat DetectLane::preProcess(const Mat &src)
{
    Mat imgThresholded, imgHSV, dst, gray, xsobel, ysobel, imgThresholded2;
    /*cvtColor(src, gray, COLOR_BGR2GRAY);
    GaussianBlur( gray, dst, Size( 5, 5 ), 0, 0 );
    Mat sobelx = (Mat_<float>(3,3) <<  0, 1, 1 ,  -1, 0, 1 ,  -1, -1, 0 );
    Mat sobely = (Mat_<float>(3,3) << -1, -1, 0 , -1, 0, 1 ,  0, 1, 1);

    Point anchor = Point( -1, -1 );
    //filter2D(dst, xsobel, -1 , sobelx, anchor, 0, BORDER_DEFAULT );
    //filter2D(dst, ysobel, -1 , sobely, anchor, 0, BORDER_DEFAULT );
    Sobel( dst, xsobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT );
    Sobel( dst, ysobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT );
    bitwise_or(xsobel, ysobel, dst);
  
    //threshold(dst,dst,70,255,0);
    //erode(dst, dst, Mat(), Point(-1, -1), 1, 1, 1);
   // dilate(dst, dst, Mat(), Point(-1, -1), 2, 1, 1);
*/
    cvtColor(src, imgHSV, COLOR_BGR2HSV);
    //------chay thuong xuyen-------
    inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]), 
        Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]), 
        imgThresholded);
    //-----shadow---------------- 
/*
    inRange(imgHSV, Scalar(0, 0, 120), 
        Scalar(179, 255, 255), 
        imgThresholded2);
    //---shadow check--------------------------
    int count =0;
    
    
    for(int i=0; i <= 640; i++)
    {
        if(imgThresholded2.at<uchar>(420,i) !=0 ) count++;
        
    }
    if (count < 30) {
        imgThresholded = imgThresholded2;
        //cout << " shadow" << endl;
    }
   
   // line(imgThresholded2, cv::Point(1, 420), cv::Point(640, 420), cv::Scalar(255, 255, 255), 3);
    //imshow("shadow", imgThresholded2); 
*/
    //-----------------------------------------------
    GaussianBlur( imgThresholded, dst, Size( 5, 5 ), 0, 0 ); 
    threshold( dst, dst, 250, 255,0);  
    if ( DetectLane::imgCount == 3 ){
	if ( bien_phai < 500) hcn = 160; 
	else hcn = 500 - bien_phai;
	if (DetectLane::phai == 1)  
	    rectangle(dst, Rect(0, 410, 360 - hcn, 70), cv::Scalar(0, 0, 0), -1); // ve hinh chu nhat mau den
    }
    else{   
	    if ( bien_phai < 500) hcn = (500 - bien_phai)*1.5;
	    else hcn = 500 - bien_phai;
	    if (DetectLane::phai == 1)  
	    rectangle(dst, Rect(0, 410, 360 - hcn, 70), cv::Scalar(0, 0, 0), -1); // ve hinh chu nhat mau den
    }
    if ( DetectLane::imgCount1 == 1)
    line(dst, Point(639, 200), Point(639, 460), Scalar(255, 255, 255), 3);
    if ( DetectLane::imgCount1 == 2)
    line(dst, cv::Point(1, 200), cv::Point(1, 460), cv::Scalar(255, 255, 255), 3);
    return dst;
}


void DetectLane::fillLane(Mat &src)
{
    vector<Vec4i> lines;
    HoughLinesP(src, lines, 1, CV_PI/180, 1);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line(src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 3, CV_AA);
    }
}


Mat DetectLane::morphological(const Mat &img)
{
    Mat dst;

    
    erode(img, dst, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
    dilate(dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
   

    // blur(dst, dst, Size(3, 3));

    return dst;
}



Mat DetectLane::birdViewTranform(const Mat &src)
{
    Mat lambda( 2, 4, CV_32FC1 );
     //Mat dst;
    Mat dst = Mat(480, 640, CV_8UC1);
    lambda = Mat::zeros( dst.rows, dst.cols, dst.type() );

    Point2f inputQuad[4]; 
    Point2f outputQuad[4];

    inputQuad[0] = Point2f( 100,150 );      // 1 239
    inputQuad[1] = Point2f( 620,150);       // 319 239
    inputQuad[2] = Point2f( 639,460);        // 209 120
    inputQuad[3] = Point2f( 100, 460 );       //50 120

    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( 640,0);
    outputQuad[2] = Point2f( 640,480);
    outputQuad[3] = Point2f( 0,480 );

    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    warpPerspective(src,dst,lambda,dst.size() );
    return dst;
    
}
