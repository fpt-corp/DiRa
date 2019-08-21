#ifndef SIGN_DETECTOR_H
#define SIGN_DETECTOR_H

#include <opencv2/opencv.hpp>
#include "LaneDetector.h"
#include "Utilities.h"

namespace sd
{
    extern Mat leftSign, rightSign, stopSign;
    extern int sign;
    void init();
    double similar(Mat &img1, Mat &img2);
    void equalizeHistBGR(Mat &src, Mat &dst);
    double distance(Point p1, Point p2);
    void DetectSign(Mat &color, Mat &depth);
    int recognizeSign(Mat &sign);
}

#endif
