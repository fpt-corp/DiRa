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
int DetectLane::da = 0;
Point DetectLane::null = Point();
  Mat img_hsv;
DetectLane::DetectLane() {
    cvCreateTrackbar("LowH", "Threshold", &minThreshold[0], 179 );
    cvCreateTrackbar("HighH", "Threshold", &maxThreshold[0], 179);

    cvCreateTrackbar("LowS", "Threshold", &minThreshold[1], 255);
    cvCreateTrackbar("HighS", "Threshold", &maxThreshold[1], 255);

    cvCreateTrackbar("LowV", "Threshold", &minThreshold[2], 255);
    cvCreateTrackbar("HighV", "Threshold", &maxThreshold[2], 255);
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
    Mat src_clone = src.clone(); 
    cvtColor(src_clone,img_hsv,CV_BGR2HSV);
   
    //setMouseCallback("HSV",mouse_callback,&img_hsv);
    imshow("HSV", img_hsv);

    vector<Mat> layers1 = splitLayer(img);
    vector<vector<Point> > points1 = centerRoadSide(layers1);
    detectLeftRight(points1);
    Mat birdView, lane;
    birdView = Mat::zeros(img.size(), CV_8UC3);
    lane = Mat::zeros(img.size(), CV_8UC3);
    for (int i = 0; i < points1.size(); i++)
     {
        for (int j = 0; j < points1[i].size(); j++)
        {
            circle(birdView, points1[i][j], 1, Scalar(0,0,255), 2, 8, 0 );
        }
    }
    imshow("Debug", birdView);
    for (int i = 1; i < leftLane.size(); i++)
    {
        if (leftLane[i] != null)
        {
            circle(lane, leftLane[i], 1, Scalar(0,0,255), 2, 8, 0 );
        }
    }

    for (int i = 1; i < rightLane.size(); i++)
    {
        if (rightLane[i] != null) {
            circle(lane, rightLane[i], 1, Scalar(255,0,0), 2, 8, 0 );
            circle(lane, (rightLane[i] + leftLane[i])/2 , 1, Scalar(255,255,0), 2, 8, 0 );
            
        }
    }

    
    imshow("Lane Detect", lane);
    int DotLef,DotRigh,TamDoan[20]={0};
	unsigned char nguong_trang,nline;
    nguong_trang = 150; //chon nguong trang
    TamDoan[0] = img.cols/2;      //Tam duong ban dau
    nline = 1;
    leftLane.clear();
    rightLane.clear();
    centerLane.clear();
    for (int ilr = 0; ilr < 16; ilr++)
    {
        leftLane.push_back(null);
        rightLane.push_back(null);
		centerLane.push_back(null);
    }
    
    for(int i=img.rows-30;i>img.rows-185;i-=10)
    {
        // bien trai
        for(int j=TamDoan[nline-1];j>6;j--)
        {
            if(img.at<uchar>(i,j) > nguong_trang)
            {
                DotLef = j;
                j = 6;
            }
            if(j == 7) 
            {
                DotLef = 0;
            }
        }
        // bien phai
        for(int j=TamDoan[nline-1];j<img.cols-6;j++)
        {
            if(img.at<uchar>(i,j) > nguong_trang)
            {
                DotRigh = j;
                j=img.cols-6;
            }
            if(j == (img.cols-7))
            {
                DotRigh = img.cols-1;
            }
        }

        TamDoan[nline] = (DotRigh + DotLef)/2;
        leftLane[nline-1] = Point2i(DotLef, i);
		rightLane[nline-1] = Point2i(DotRigh, i);
		centerLane[nline-1]= (Point2i(DotLef, i)+Point2i(DotRigh, i))/2;
        nline += 1;
    }

    Mat lane;
    lane = Mat::zeros(img.size(), CV_8UC3);
    for (int i = 0; i < leftLane.size(); i++)
    {
        if (leftLane[i] != null)
        {
            circle(lane, leftLane[i], 1, Scalar(0,0,255), 2, 8, 0 );
        }
    }
    for (int i = 0; i < rightLane.size(); i++)
    {
        if (rightLane[i] != null) {
            circle(lane, rightLane[i], 1, Scalar(255,0,0), 2, 8, 0 );
        }
    }
    for (int i = 0; i < centerLane.size(); i++)
    {
        if (centerLane[i] != null) {
            circle(lane, centerLane[i], 1, Scalar(0,255,0), 2, 8, 0 );
        }
    }
    imshow("Lane Detect", lane);
    imshow("img", img);
}

Mat DetectLane::preProcess(const Mat &src)
{
    Mat imgThresholded, imgHSV, dst;
    cvtColor(src, imgHSV, COLOR_BGR2HSV);
    inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]), 
        Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]), 
        imgThresholded);
    GaussianBlur( imgThresholded, dst, Size( 5, 5 ), 0, 0 );
    dst = birdViewTranform(dst);
    fillLane(dst);
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
