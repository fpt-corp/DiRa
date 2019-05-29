#include"detectsign.h"
DetectSign::DetectSign(const string svmModel)
{
    hog = HOGDescriptor(Size(32,32), Size(16,16), Size(8,8), Size(8,8), 9);
    svm = SVM::load(svmModel);
}
vector<Mat> DetectSign::MSER_Features(const Mat& src) 
{
    
    cv::Mat detection;
    cv::Size size(32, 32);
    std::vector<cv::Mat> detections;
    detections.clear();
    boxes.clear();

    Mat hsvImg;
    Mat dst = src.clone();

    cvtColor(dst, hsvImg, CV_BGR2HSV);
  
    Mat binImg;
    inRange(hsvImg, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]) , 
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2] ),binImg);
    imshow("binImg", binImg);

    // MSER regions detection
    cv::Ptr<cv::MSER> ms = cv::MSER::create(500, 10, 14400, 0.9,
        0.1, 200, 1.01, 0.1, 1);
    std::vector<std::vector<cv::Point> > regions;
    std::vector<cv::Rect> mser_bbox;
    ms->detectRegions(binImg, regions, mser_bbox);

    // For every bounding box in the image
    for (cv::Rect i : mser_bbox) {
        // Ratio filter of detected regions
        double ratio = (static_cast<double>(i.height) /
            static_cast<double>(i.width));

        if (ratio > 0.7 && ratio < 1.3) {
            // Crop bounding boxes to get new images
            detection = src(i);

            // Resize images  to fit the trained data
            cv::resize(detection, detection, size);

            // Output the vector of images
            detections.push_back(detection);
            boxes.push_back(i);
        }
    }

    for(int i = 0 ; i < boxes.size() - 1; i++)
        for(int j = i + 1; j < boxes.size(); j++)
            if(boxes[i].area() < boxes[j].area() )
                {
                    swap(boxes[i], boxes[j]);
                    swap(detections[i], detections[j]);
                }    
    return detections;
}

bool DetectSign::detect(const Mat &binImg)
{
    int width = binImg.size().width;
    int height = binImg.size().height;
    vector< vector<Point> > contours;
    contours.clear();
    rects.clear();
    vector<Vec4i> hierarchy;

    findContours(binImg, contours, hierarchy, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    bool detected = false;
    if(contours.size() > 0)
    {

        for(size_t i = 0; i < contours.size(); i++)
        {
            Rect rect = boundingRect(contours[i]);
            double ratio = static_cast<double> (rect.width) / rect.height; // ratio width/height
            double ratioRect = static_cast<double> (rect.area())/(width*height);
            if(ratioRect > MIN_AREA) // Check if ratioRect is too small?
                if(ratio > 0.5 && ratio < 1.5)
                    {   
                        detected = true;
                        rects.push_back(rect);   
                    }    
        }
        
    }
    return detected;
}
int DetectSign::compareVector(const Mat &grayImg)
{
    int flag = 0;
    for(size_t i = 0; i < rects.size(); i++)
    {
        Rect rect = rects[i];

        Mat graySign;
        resize(grayImg(rect),graySign, Size(32,32)); //Resize gray image to fit SVM model
        //Compute HOG
        vector<float> descriptors;
        hog.compute(graySign, descriptors);

        Mat vec(descriptors, CV_32F);

        int classSign = static_cast<int>(svm->predict(vec.t()));

        if(classSign == 1) //Turn left sign
            flag = 1;
        if(classSign == 2) //Turn right sign
            flag = 2;

        if(flag != 0)
        {
            rectSign = rect;
            return flag;
        }
    }
    return flag;
}
int DetectSign::update(const Mat &src)
{
    Mat hsvImg, grayImg;
    Mat dst = src.clone();

    cvtColor(dst, hsvImg, CV_BGR2HSV);
    cvtColor(dst, grayImg, CV_BGR2GRAY);
  
    Mat binImg;
    inRange(hsvImg, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]) , 
                Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2] ),binImg);
    
    if(detect(binImg))
        return compareVector(grayImg);
    else return 0;
}
Rect DetectSign::drawSign()
{
    return rectSign;
}
