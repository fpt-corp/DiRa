#ifndef DETECTLANE_H
#define DETECTLANE_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "std_msgs/String.h"
#include <vector>
#include <math.h>
#include <algorithm>

#include "carcontrol.h"

#define PI 3.14f

using namespace std;
using namespace cv;

class DetectLane
{
	public:
		
		DetectLane();
		~DetectLane();
		vector<Point> laneLeft, laneRight;
		void update(const Mat &src);
		void detect(const Mat &src);
		vector<Point> getLeftLanes();
		vector<Point> getRightLanes();
		
		
		
	private:
		Mat preProcess(const Mat &src);
		Mat laneInShadow(const Mat &src);
		Mat detectLane(const Mat &src);
		Mat birdViewTranform(const Mat &src);
		Mat View(const Mat &src);
		Mat morphological(const Mat &img);
		Mat dilateLane(const Mat &src);
		Mat erodeLane(const Mat &src);
	
		Mat reduceNoise(const Mat& src);
		Mat reduceNoiseBinImg(const Mat& binImg);
		Mat LaplacianEdgeDetect(const Mat& grayImg);

		Mat convertBGR2HSV(const Mat& src);
		Mat convertBGR2HLS(const Mat& src);
		Mat sobelEdgeDetect(const Mat& src_gray);
		Mat cannyEdgeDetect(const Mat &grayImg);
		



		int ignoreMid(int x);
		void findLane(const Mat &BinarySrc);
		
		double a = (double) 120/320 , b = 30 ;
		
		const Size Scan = Size(4, 4);
		int tolerance = 30;
		float alpha = 0.2;
		int minThreshold[3] = {0, 0, 180};
		int maxThreshold[3] = {180,255,255};
		int minShadowTh[3] = {90, 50, 40};
    	int maxShadowTh[3] = {120, 255, 180};
    	int minLaneInShadow[3] = {0, 0, 100};
		int maxLaneInShadow[3] = {180, 255, 255};

		int skyLine = 240;

		int KernelSize = 3;
		int K_MEDIAN = 5;
		int NGUONG_THRESHOLD = 100;

		bool isLeft, isRight;
		
		
		
};

#endif
