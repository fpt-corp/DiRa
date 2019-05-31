#ifndef IO_INTERFACE_H
#define IO_INTERFACE_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>

using namespace std;
using namespace cv;

class IOInterface
{
public:
    IOInterface(string ip="127.0.0.1");
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
    int clientSd;
    float steer,speed,cam_angle;
    Mat bgr,depth;
    Vec3f imu_pose;
    thread tr;
    char msg[50000];
    void getDataThreadFunc();
    bool ConnectSimulator(string serverIp, int port);
    float bytesToFloat(char msg[],int begin);
};
#endif