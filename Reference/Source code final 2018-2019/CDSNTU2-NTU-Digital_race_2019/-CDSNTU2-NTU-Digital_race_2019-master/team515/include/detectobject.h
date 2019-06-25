#ifndef DETECTOBJECT_H
#define DETECTOBJECT_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/dnn/dnn.hpp>

using namespace cv;
using namespace cv::dnn;
using namespace std;

class DetectObject{
public:
    
    DetectObject(const string path);
    ~DetectObject();

    void update(const Mat &src);

    vector<int> ClassIds;
    vector<Rect> Data;

private:
    float confThreshold;
    float nmsThreshold;
    int inpWidth;
    int inpHeight;

    vector<string> classes;
    string classesFile;
    string modelConfiguration;
    string modelWeights;
    Net net;
    
    void postprocess(Mat& frame, const vector<Mat>& outs);
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
    
    
    vector<String> getOutputsNames(const Net& net);
};


#endif
