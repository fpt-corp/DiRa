#include "objectdetect.h"
 
ObjectDetect::ObjectDetect()
{
    confThreshold = 0.5;
    nmsThreshold = 0.4;
    inpWidth = 448;
    inpHeight = 448;

    classesFile = "/home/kento/catkin_ws/src/lane_detect/data/objects.names";
    modelConfiguration = "/home/kento/catkin_ws/src/lane_detect/data/objects-yolov3-tiny.cfg";
    modelWeights = "/home/kento/catkin_ws/src/lane_detect/data/objects-yolov3-tiny_10000.weights";

    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line))
    {
        cerr<<line<<endl;
        classes.push_back(line);
    }

    net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
}
ObjectDetect::~ObjectDetect() {}

void ObjectDetect::update(const Mat &src)
{
    Mat blob;
    Mat frame = src.clone();

    blobFromImage(frame, blob, 1/255.0, Size(inpWidth, inpHeight), Scalar(0,0,0), true, false);

    net.setInput(blob);
    
    vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));

    postprocess(frame, outs);
    
    Mat detectedFrame;
    frame.convertTo(detectedFrame, CV_8U);

    imshow("Object_View",frame);
}

void ObjectDetect::postprocess(Mat& frame, const vector<Mat>& outs)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    
    for (size_t i = 0; i < outs.size(); ++i)
    {
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;

            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }
    
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    Data = boxes;
    ClassIds = classIds;

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }

}

void ObjectDetect::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0,255,0), 3);
    
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
        cerr<<label<<endl;
    }

    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top), Scalar(0,255,0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255,255,255),1);
}

vector<String> ObjectDetect::getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        vector<int> outLayers = net.getUnconnectedOutLayers();
        
        vector<String> layersNames = net.getLayerNames();
        
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}
