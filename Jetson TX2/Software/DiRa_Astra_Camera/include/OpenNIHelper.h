#ifndef OPENNI_HELPER_H
#define OPENNI_HELPER_H

#include "opencv2/opencv.hpp"
#include "OpenNI.h"

namespace ni
{
    extern openni::Device device;
    extern openni::VideoStream color, depth;
    extern openni::VideoFrameRef colorFrame, depthFrame;

    void openni2_init();
    void openni2_getmat(cv::Mat &color, cv::Mat &depth);
    void openni2_destroy();
}

#endif