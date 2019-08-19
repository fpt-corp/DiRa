#include "bien_bao.h"
#include "Utilities.h"

Mat leftSign = imread("/home/nvidia/catkin_ws/src/bienbao/img/trai.png", CV_LOAD_IMAGE_COLOR);
Mat rightSign = imread("/home/nvidia/catkin_ws/src/bienbao/img/phai.png", CV_LOAD_IMAGE_COLOR);
Mat truong,manhha;
int bien_bao::bien_b=0;
int kt_tr(0),kt_ph(0),kt_no(0),kq(0);

bien_bao::bien_bao(){
    bienbao_publisher = bb.advertise<std_msgs::Float32>("set_bien_bao",10);
}
bien_bao::~bien_bao(){}

void equalizeHistBGR(Mat &src, Mat &dst)
{
    if (src.channels() >= 3)
    {
        Mat ycrcb;

        cvtColor(src, ycrcb, CV_BGR2YUV);

        vector<Mat> channels;
        split(ycrcb, channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels, ycrcb);

        cvtColor(ycrcb, dst, CV_YUV2BGR);
    }
}
double distance(Point p1, Point p2)
{
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

double similar(Mat &img1, Mat &img2)
{
    Mat hsv1, hsv2;
    if (img1.empty())
    {
        // cout << "empty" << endl;
        return 0;
    }
    resize(img1, hsv1, img1.size());
    equalizeHistBGR(hsv1, hsv1);
    cvtColor(hsv1, hsv1, COLOR_BGR2HSV);
    resize(img2, hsv2, hsv1.size());
    cvtColor(hsv2, hsv2, COLOR_BGR2HSV);
    int d = 0, s = 0;
    for (size_t i = 0; i < hsv1.cols; i++)
    {
        for (size_t j = 0; j < hsv1.rows; j++)
        {
            if (distance(Point(i, j), Point(trunc(hsv1.cols / 2), trunc(hsv1.rows / 2))) <= hsv1.rows / 2)
            {
                s++;
                if (hsv1.at<Vec3b>(j, i)[1] > 70 && hsv1.at<Vec3b>(j, i)[2] > 30)
                {
                    if (abs(hsv2.at<Vec3b>(j, i)[0] - hsv1.at<Vec3b>(j, i)[0]) < 15)
                    {
                        d++;
                    }
                }
                else
                {
                    if (abs(hsv2.at<Vec3b>(j, i)[2] - hsv1.at<Vec3b>(j, i)[2]) < 10)
                    {
                        d++;
                    }
                }
            }
        }
    }
	   return (d * 1.0 / s);
}

int recognizeSign(Mat &sign)
{
	
    double p_left = similar(sign, truong);

    double p_right = similar(sign, manhha);
    //double p_stop = similar(sign, stopSign);
    double p_max = max(p_left,p_right);

    if (p_max < 0.5) return 0;
    // cout << p_max << " ";
    if (p_max == p_left) return 1;
    if (p_max == p_right) return 2;
    
}

Mat bien_bao::birdViewTranform(const Mat &src)
{
    Mat lambda( 2, 4, CV_32FC1 );
     //Mat dst;
    Mat dst = Mat(480, 640, CV_8UC1);
    lambda = Mat::zeros( dst.rows, dst.cols, dst.type() );

    Point2f inputQuad[4]; 
    Point2f outputQuad[4];

    inputQuad[0] = Point2f( 250,1 );      // 1 239
    inputQuad[1] = Point2f( 500,1);       // 319 239
    inputQuad[2] = Point2f( 540,240);        // 209 120
    inputQuad[3] = Point2f( 250, 240 );       //50 120

    outputQuad[0] = Point2f( 0,0 );
    outputQuad[1] = Point2f( 640,0);
    outputQuad[2] = Point2f( 640,480);
    outputQuad[3] = Point2f( 0,480 );

    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    warpPerspective(src,dst,lambda,dst.size() );
    return dst;
    
}

void bien_bao::bienbao(const Mat &src)
{
    CascadeClassifier face_cascade;
    if(!face_cascade.load("/home/nvidia/catkin_ws/src/bienbao/cascade.xml")) {
        cout<<"Error loading cascade file for face";
    }
    if(src.empty())
   	{
        	cout <<"error ";
   	}

    Mat gray_img; Mat subR;
	vector<Rect> faces;
	cvtColor(src, gray_img, CV_BGR2GRAY);
	face_cascade.detectMultiScale(gray_img, faces, 1.055, 1, CV_HAAR_SCALE_IMAGE | CV_HAAR_DO_CANNY_PRUNING, cvSize(0,0), cvSize(300,300));
    for(int i=0; i < faces.size();i++)
        {
            Point pt1(faces[i].x+faces[i].width, faces[i].y+faces[i].height);
            Point pt2(faces[i].x,faces[i].y);
            Mat faceROI = src(faces[i]);
            subR = faceROI.clone();
            imshow("b",faceROI);
            rectangle(src, pt1, pt2, cvScalar(0,255,0), 2, 8, 0);
        }
    imshow("a", src);
    
  
    if(!subR.empty()) 
    {
			//
			//imshow("bb",subR);
			//cv::waitKey(30);
            
		resize(leftSign,truong,subR.size(),1,1,INTER_CUBIC);
		resize(rightSign,manhha,subR.size(),1,1,INTER_CUBIC);
            //imshow("ph",manhha);
    		//imshow("tr",truong);
		int vunhomz=recognizeSign(subR);
            //cout << vunhomz<<endl;
			
		if (vunhomz==1) kt_tr++; 
		if (vunhomz==2) kt_ph++;
		if (vunhomz==0) kt_no++;
            //cout<<kt_tr<< " "<< kt_ph<<" "<<kt_no<< "   \n  ";
		if (kt_tr>10) 
		{
			kt_tr=0;
			kt_ph=0;
			kt_no=0;
			bien_bao::bien_b = kq=2; 
            subR=subR.empty();
		}
		if(kt_ph>10)
		{
			kt_tr=0;
			kt_ph=0;
			kt_no=0;
			bien_bao::bien_b =  kq=1;
            subR=subR.empty();
		}
		if (kt_no>10) 
		{
			kt_tr=0;
			kt_ph=0;
			kt_no=0;
			kq=0;
            subR=subR.empty();
		}
		cout<<kq<< "   \n  ";
            

		
	}
	
	
  
    std_msgs::Float32 dem; 
    dem.data = bien_bao::bien_b;   
    bienbao_publisher.publish(dem);
}
