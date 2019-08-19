#ifndef UTILITIES_H
#define UTILITIES_H

#include "opencv2/opencv.hpp"
#define PI 3.14159
#define GROUND_PLANE_INPUT "ground_plane.txt"

namespace utl
{
extern cv::Vec4f groundPlane;

double computeAngle(cv::Point A, cv::Point O, cv::Point B);
float dToPlane(cv::Point3f p, cv::Vec4f plane);
cv::Vec4f findPlaneEquation(std::vector<cv::Point3f> _3Point);
cv::Point3f getRealPointInWorld(cv::Point screenPoint, int depth);
void readGroundPlane();
}

#endif
