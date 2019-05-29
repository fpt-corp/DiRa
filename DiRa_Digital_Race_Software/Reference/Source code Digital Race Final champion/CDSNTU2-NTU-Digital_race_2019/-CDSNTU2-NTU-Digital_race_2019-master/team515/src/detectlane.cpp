#include"detectlane.h"
DetectLane::DetectLane(){ 
	//Threshold
    cvCreateTrackbar("LowH", "Threshold", &minThreshold[0], 179);
    cvCreateTrackbar("HighH", "Threshold", &maxThreshold[0], 179);

    cvCreateTrackbar("LowS", "Threshold", &minThreshold[1], 255);
    cvCreateTrackbar("HighS", "Threshold", &maxThreshold[1], 255);

    cvCreateTrackbar("LowV", "Threshold", &minThreshold[2], 255);
    cvCreateTrackbar("HighV", "Threshold", &maxThreshold[2], 255);

    cvCreateTrackbar("NGUONG", "Threshold", &NGUONG_THRESHOLD, 255);

}
DetectLane::~DetectLane(){}
//--------------------------Cac ham xu ly lay lane duong -----------------------------------
Mat DetectLane::preProcess(const Mat &src) //Input: Original Image, Output: Threshold Image
{
    Mat imgThresholded, imgHSV, dst;

    cvtColor(src, imgHSV, COLOR_BGR2HSV);

    inRange(imgHSV, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]), 
        Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]), 
        imgThresholded);
	//imshow("Threshold Origin", imgThresholded);
    return imgThresholded;
}
Mat DetectLane::View(const Mat &src) //Input: Image , Output: Half Image
{
	int width = src.size().width;
	int height = src.size().height;
	Mat dst = src.clone();
	Rect view(0, height/2, width, height/2);
	Mat ROI = dst(view);
	return ROI;
}
Mat DetectLane::morphological(const Mat &img)//Input: Image, Output: Image with dilate method
{
    	Mat dst;
	int erosion_size = 4;
	cv::Mat element = cv::getStructuringElement(MORPH_ELLIPSE,Size(2 * erosion_size + 1, 2 * erosion_size + 1),Point(erosion_size, erosion_size)); 
	dilate(img, dst, element );
    //erode(dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(9, 9), Point(4,4)) );
    	return dst;
}
Mat DetectLane::dilateLane(const Mat &src)
{
	Mat dst;
	int erosion_size = 4;
	cv::Mat element = cv::getStructuringElement(MORPH_ELLIPSE,Size(2 * erosion_size + 1, 2 * erosion_size + 1),Point(erosion_size, erosion_size));
	dilate(src, dst, element);
	return dst;
}
Mat DetectLane::erodeLane(const Mat &src)
{
	Mat dst;
	int erosion_size = 4;
	cv::Mat element = cv::getStructuringElement(MORPH_ELLIPSE,Size(2 * erosion_size + 1, 2 * erosion_size + 1),Point(erosion_size, erosion_size));
	erode(src, dst, element);
	return dst;
}
Mat DetectLane::reduceNoise(const Mat& src) //Input: RGB Image, Output: Gray Image with Median Filter
{
	Mat dst;
	cvtColor(src, dst, COLOR_RGB2GRAY);
	medianBlur(dst, dst, K_MEDIAN);
	return dst;
}
Mat DetectLane::LaplacianEdgeDetect(const Mat& grayImg) //Input : GrayImg, Output : Bin Image with Laplacian and Threshold
{
	Mat dst;
	Laplacian(grayImg, dst, CV_32F, KernelSize);
	convertScaleAbs(dst, dst);
	threshold(dst, dst, NGUONG_THRESHOLD, 255, THRESH_BINARY);
	return dst;
}
Mat DetectLane::convertBGR2HSV(const Mat& src)
{
    int lower[3] = {0, 0, 180};
    int upper[3] = {180, 255, 255};
    Mat hsvImg, binImg;
    cvtColor(src, hsvImg, CV_BGR2HSV);
    inRange(hsvImg, Scalar(lower[0], lower[1], lower[2]), Scalar(upper[0], upper[1], upper[2]), binImg);
    return binImg;
}
Mat DetectLane::convertBGR2HLS(const Mat&src)
{
    int lower_white[3] = {0,180,0};
    int upper_white[3] = {255,255,255};
    Mat hlsImg, binImg;
    cvtColor(src, hlsImg, CV_BGR2HLS);
    inRange(hlsImg, Scalar(lower_white[0], lower_white[1], lower_white[2]), 
        Scalar(upper_white[0], upper_white[1], upper_white[2]), binImg);
    return binImg;
}
Mat DetectLane::sobelEdgeDetect(const Mat& src_gray)
{
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Mat grad;

    /// Gradient X
    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    return grad;
}
Mat DetectLane::cannyEdgeDetect(const Mat &grayImg)
{
    Mat edge, dst;
    blur( grayImg, edge, Size(3,3) );
    int ratio = 3;
    int kernel_size = 3;
    int lowThreshold = 100;
    /// Canny detector
    Canny( edge, dst, lowThreshold, lowThreshold*ratio, kernel_size );
    return dst;
}
Mat DetectLane::reduceNoiseBinImg(const Mat& binImg) //input: Binary Image, Output: Binary Image with Median Filter
{
    Mat dst;
    medianBlur(binImg, dst, 5);
    return dst;
}
Mat DetectLane::laneInShadow(const Mat &src)
{
    //int minShadowTh[3] = {90, 50, 40}; //Doi S va V
    //int maxShadowTh[3] = {120, 255, 180}; //Doi S va V
    //int minLaneInShadow[3] = {0, 0, 100};
    //int maxLaneInShadow[3] = {180, 255, 255};
    Mat shadowMask, shadow, imgHSV, shadowHSV, laneShadow;
    cvtColor(src, imgHSV, COLOR_BGR2HSV);

    inRange(imgHSV, Scalar(minShadowTh[0], minShadowTh[1], minShadowTh[2]),
    Scalar(maxShadowTh[0], maxShadowTh[1], maxShadowTh[2]),  
    shadowMask);
    imshow("shadowMask", shadowMask);
    src.copyTo(shadow, shadowMask);
    imshow("shadow",shadow);
    cvtColor(shadow, shadowHSV, COLOR_BGR2HSV);

    inRange(shadowHSV, Scalar(minLaneInShadow[0], minLaneInShadow[1], minLaneInShadow[2]), 
        Scalar(maxLaneInShadow[0], maxLaneInShadow[1], maxLaneInShadow[2]), 
        laneShadow);
    imshow("laneShadow", laneShadow);
    return laneShadow;
}
Mat DetectLane::detectLane(const Mat &src)
{
	Mat src1 = src.clone();
	Mat src2 = src.clone();

	src1 = preProcess(src1);
	src2 = laneInShadow(src2);
	Mat dst;
	bitwise_or(src1, src2, dst);
	//imshow("Lane combine with shadow",dst);
	return dst;
}
//-----------------------------Cac ham xu ly diem duong---------------------------------------------
int DetectLane::ignoreMid(int x)
{
	return int(a * x + b);
}

//---------------New Method---------------------------------------

void DetectLane::findLane(const Mat &binImg)
{
	laneLeft.clear();
	laneRight.clear();
	
	
	int left = 0;
	int right = 0;

	int midLane = binImg.cols / 2 ;
	int start = binImg.rows - Scan.height;

	for (int high = start; high > 20; high = high - Scan.height) // with a line
	{
		// Left lane
		for (int i = midLane - ignoreMid(high); i > Scan.width; i = i - (Scan.width / 2))
		{
			if( i > binImg.cols - Scan.width || high > binImg.rows - Scan.height || i < 0 || high < 0)
				continue;
			Mat scan = Mat(binImg, Rect(i, high, Scan.width, Scan.height));
			if (countNonZero(scan) * 100 > Scan.area() * tolerance)
			{
				laneLeft.push_back(Point(i, high));
				left = i;
				break;
			}
			left = 0;
		}
		// Right lane
		for (int i = midLane + ignoreMid(high); i < binImg.cols - Scan.width; i = i + (Scan.width / 2))
		{
			if( i > binImg.cols - Scan.width || high > binImg.rows - Scan.height || i < 0 || high < 0)
				continue;
			Mat scan = Mat(binImg, Rect(i, high, Scan.width, Scan.height));
			if (countNonZero(scan) * 100 > Scan.area() * tolerance)
			{
				laneRight.push_back(Point(i, high));
				right = i;
				break;
			}
			right = 0;
		}
		// Update new mid lane
		if (left * right > 0)
		{
			midLane = midLane * alpha + ((left + right) / 2) * (1 - alpha);
			continue;
		}
		if (left + right == 0)
			continue;
		if (left == 0)
			left = midLane - ignoreMid(high) * 2;
		if (right == 0)
			right = midLane + ignoreMid(high) * 2;
		midLane = midLane * alpha + ((left + right) / 2) * (1 - alpha);
	}
}
vector<Point> DetectLane::getLeftLanes()
{
    return laneLeft;
}

vector<Point> DetectLane::getRightLanes()
{
    return laneRight;
}
void DetectLane::update(const Mat &src)
{
	Mat binImg;
	binImg = reduceNoise(src).clone();
	binImg = LaplacianEdgeDetect(binImg);
	imshow("binImg",binImg);
	binImg = View(binImg).clone();
	findLane(binImg);
	
}
