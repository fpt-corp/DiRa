#include "signclassify.h"

SignClassify::SignClassify()
{
    left_cascade.load("/home/kento/catkin_ws/src/lane_detect/data/left_cascade.xml");
    right_cascade.load("/home/kento/catkin_ws/src/lane_detect/data/right_cascade.xml");
}

SignClassify::~SignClassify() {}

int SignClassify::classify(const Mat &src)
{
    vector<Rect> Rec;
    left_cascade.detectMultiScale(src,Rec,1.05);
    if((int)Rec.size() > 0) return 0;
    right_cascade.detectMultiScale(src,Rec,1.05);
    if((int)Rec.size() > 0) return 1;
    return -1;
}

int SignClassify::cropImage(const Mat &img_input, Mat &img_output, Rect roi)
{
    roi.x = max(roi.x,0);
    roi.y = max(roi.y,0);
    roi.height = min(roi.height,img_input.rows - roi.y);
    roi.width = min(roi.width,img_input.cols - roi.x);
    if(!(0 <= roi.x && 0 <= roi.width && roi.x + roi.width <= img_input.cols && 
        0 <= roi.y && 0 <= roi.height && roi.y + roi.height <= img_input.rows)) return -1;
    img_output = img_input(roi);
    return 0;
}
