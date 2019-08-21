#include "OpenNIHelper.h"

openni::Device ni::device;
openni::VideoStream ni::color, ni::depth;
openni::VideoFrameRef ni::colorFrame, ni::depthFrame;

void ni::openni2_init()
{
    int raw_width = 320, raw_height = 240, fps = 30;
    openni::OpenNI::initialize();
    device.open(openni::ANY_DEVICE);
	openni::VideoMode depthVM, colorVM;

	depthVM.setFps(fps);
	depthVM.setResolution(raw_width, raw_height);
	depthVM.setPixelFormat(openni::PIXEL_FORMAT_DEPTH_1_MM);

	colorVM.setFps(fps);
	colorVM.setResolution(raw_width, raw_height);
	colorVM.setPixelFormat(openni::PIXEL_FORMAT_RGB888);

    color.create(device, openni::SENSOR_COLOR);
    color.setVideoMode(colorVM);
    color.start();

    depth.create(device, openni::SENSOR_DEPTH);
    depth.setVideoMode(depthVM);
    depth.start();

    color.setMirroringEnabled(false);
    depth.setMirroringEnabled(false);

    device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
    device.setDepthColorSyncEnabled(true);
}

void ni::openni2_getmat(cv::Mat &mat_color, cv::Mat &mat_depth)
{
    color.readFrame(&colorFrame);
    mat_color = cv::Mat(240, 320, CV_8UC3, (uint8_t*)colorFrame.getData());
    cv::cvtColor(mat_color, mat_color, CV_BGR2RGB);

    depth.readFrame(&depthFrame);
    mat_depth = cv::Mat(240, 320, CV_16UC1, (openni::DepthPixel*)depthFrame.getData());
}

void ni::openni2_destroy()
{
    color.stop();
    color.destroy();
    depth.stop();
    depth.destroy();
    device.close();
}