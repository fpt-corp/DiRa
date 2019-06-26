#include "tree.h"

float bien_phai = 0;
int co = 0;

Mat  img_mau = imread("/home/nvidia/catkin_ws/src/tree/img/n.png"); // anh mau
//Mat rightSign = imread("/home/nvidia/Documents/img/phai.png", CV_LOAD_IMAGE_COLOR);


tree::tree(){
    tree_publisher = t.advertise<std_msgs::Float32>("set_tree",10);
}
tree::~tree(){}


Mat tree::birdViewTranform(const Mat &src)
{
    Mat lambda( 2, 4, CV_32FC1 );
     //Mat dst;
    Mat dst = Mat(240, 320, CV_8UC1);
    lambda = Mat::zeros( dst.rows, dst.cols, dst.type() );

    Point2f inputQuad[4]; 
    Point2f outputQuad[4];

    inputQuad[0] = Point2f( 160,140 );      // 1 239
    inputQuad[1] = Point2f( 450,140);       // 319 239
    inputQuad[2] = Point2f( 450,210);        // 209 120
    inputQuad[3] = Point2f( 160, 210 );       //50 120

    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( 320,0);
    outputQuad[2] = Point2f( 320,240);
    outputQuad[3] = Point2f( 0,240 );

    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    warpPerspective(src,dst,lambda,dst.size() );
    return dst;
    
}

void tree::tree_a(const Mat &src)
{
    // tim mau
    Mat src_clone = src.clone();
    Mat  result;
    imshow("mau", img_mau);
    int result_cols = src_clone.cols - src_clone.cols + 1;
    int result_rows = src_clone.rows - src_clone.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);
    matchTemplate(src_clone, img_mau, result, CV_TM_CCORR_NORMED);
    threshold(result, result, 0.94, 1., CV_THRESH_TOZERO);
    
	double minval, maxval;
    int threshold = 0.95;
    Point minloc, maxloc;
    minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);
    //cout << maxval<< endl;
    if (maxval > threshold)
    {
        rectangle(
				src_clone,
				maxloc,
				cv::Point(maxloc.x + img_mau.cols, maxloc.y + img_mau.rows),
				cv::Scalar(0, 255, 0), 2
			);
        cv::floodFill(result, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));
		co = 1;
    }
    else co = 0;
    imshow("detected", src_clone);
    // 
    if ( co == 1 ){
    Mat src_hsv, birdView, imgThresholded;
    cvtColor(src, src_hsv, COLOR_BGR2HSV);
    inRange(src_hsv, Scalar(0, 0, 190), 
        Scalar(179, 255, 255), 
        imgThresholded);
    birdView = birdViewTranform(imgThresholded);
    rectangle(birdView, Rect(0, 190, 110, 50), cv::Scalar(0, 0, 0), -1); // ve hinh chu nhat mau den
    // tim diem
    for (int i = 110; i < 320; i = i + 2)
    {
        if(birdView.at<uchar>(200,i) == 255)
        {
            bien_phai = i;
            
            bien_phai = 160 + bien_phai*290/320;
            //cout<<"bien_phai: "<<bien_phai<<endl;
            i = 700;
        }
    }
        imshow("view", birdView); 
    }
    else bien_phai = 0;
    std_msgs::Float32 d; 
    d.data = bien_phai;   
    tree_publisher.publish(d);
}
