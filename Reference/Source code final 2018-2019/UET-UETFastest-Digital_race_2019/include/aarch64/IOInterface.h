#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include "Hal.h"
#include "LCDI2C.h"
#include "api_i2c_pwm.h"
#include "OpenNI.h"
#include <RTIMULib.h>
#include <thread>

#define SW1_PIN 395
#define SW2_PIN	394
#define SW3_PIN	393
#define SW4_PIN 392
#define SS1_PIN 398

using namespace std;
using namespace cv;
using namespace EmbeddedFramework;

class IOInterface
{
public:
    IOInterface();
    ~IOInterface();
    Mat getBGR();
    Mat getDepth();
    Vec3f getIMUPose();
    void setSteer(float val);
    void setSpeed(float val);
    void setCamAngle(float val);

    int chooseCarMode();
    void ready();
    
    int getLCDkey();
    void printLCD(int x,int y,string msg);
    void clearLCD();

private:
    int bn1=0,bn2=0,bn3=0,bn4=0,ss1=-1;
    GPIO *gpio;
    I2C *i2c_device;
    LCDI2C *lcd;
    PCA9685 *pca9685;
    openni::Device device;
    openni::VideoStream color, depth;
    openni::VideoFrameRef colorFrame, depthFrame;
    thread tr;
    void getIMUThreadFunc();
    Vec3f imu_pose;
};
#endif