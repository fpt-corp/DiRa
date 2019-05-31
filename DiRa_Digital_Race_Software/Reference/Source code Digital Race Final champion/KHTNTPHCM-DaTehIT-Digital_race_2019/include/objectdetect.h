#ifndef OBJECTDETECT_H
#define OBJECTDETECT_H

#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

class ObjectDetect{
public:
    ObjectDetect();
    ~ObjectDetect();

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

    void extractPlane(Mat &src, int n, int ch, Mat &dst);

    void postprocess(Mat& frame, const vector<Mat>& outs);
    void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
    vector<String> getOutputsNames(const Net& net);
};

#endif


