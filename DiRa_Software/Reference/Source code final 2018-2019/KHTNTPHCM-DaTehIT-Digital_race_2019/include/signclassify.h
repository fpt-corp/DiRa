#ifndef SIGNCLASSIFY_H
#define SIGNCLASSIFY_H

#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

class SignClassify{
public:
    SignClassify();
    ~SignClassify();

    int cropImage(const Mat &img_input, Mat &img_output, Rect roi);
    int classify(const Mat &src);
    // 0 left
    // 1 right
    // -1 undefine

private:
    CascadeClassifier left_cascade, right_cascade;
};


#endif