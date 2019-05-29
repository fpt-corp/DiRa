#ifndef DETECT_SIGN_H
#define DETECT_SIGN_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;
using namespace cv::ml;
#define MIN_AREA 0.001f

class DetectSign
{
	private:
		
		HOGDescriptor hog;
		Ptr<SVM> svm;
		int minThreshold[3] = {100, 100, 100};
		int maxThreshold[3] = {120, 255, 255};
		
		vector<Rect> boxes;
		vector<Rect> rects;

		Rect rectSign = Rect(0,0,0,0);
        vector<Mat> MSER_Features(const Mat& src);
		bool detect(const Mat &binImg);
		int compareVector(const Mat &grayImg);
		
	public:
		DetectSign(const string svmModel);
		int update(const Mat &src);
		Rect drawSign();

};
#endif
