#include "IOInterface.h"
IOInterface::IOInterface() {
    gpio = new GPIO();
    i2c_device = new I2C();
    lcd = new LCDI2C();
    gpio->gpioExport(SW1_PIN);
    gpio->gpioExport(SW2_PIN);
    gpio->gpioExport(SW3_PIN);
    gpio->gpioExport(SW4_PIN);
    gpio->gpioExport(SS1_PIN);
    gpio->gpioSetDirection(SW1_PIN, INPUT);
    gpio->gpioSetDirection(SW2_PIN, INPUT);
    gpio->gpioSetDirection(SW3_PIN, INPUT);
    gpio->gpioSetDirection(SW4_PIN, INPUT);
    gpio->gpioSetDirection(SS1_PIN, INPUT);
    i2c_device->m_i2c_bus = 1;
    i2c_device->HALOpen();
    unsigned char data;
    i2c_device->HALRead(0x3f, 0xFF, 0, &data, "");
    usleep(10000);
    lcd->LCDInit(i2c_device, 0x3f, 20, 4);
    lcd->LCDBacklightOn();
    lcd->LCDCursorOn();

    pca9685 = new PCA9685();
    api_pwm_pca9685_init(pca9685);

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

    tr=thread(&IOInterface::getIMUThreadFunc, this);
}
IOInterface::~IOInterface() {
    color.stop();
    color.destroy();
    depth.stop();
    depth.destroy();
    device.close();
    tr.join();
}
int IOInterface::getLCDkey(){
    unsigned int value;   
    gpio->gpioGetValue(SS1_PIN, &value);
    if(value!=ss1){
        ss1=value;
        if(ss1==0){
            //cout<<"che ss1"<<endl;
            return 5;
        }
        else{
            //cout<<"bo che ss1"<<endl;
            return 6;
        }
        
    }
    gpio->gpioGetValue(SW1_PIN, &value);
    if(value!=bn1){
        bn1=value;
        if(bn1==1) {
            //cout<<"bn1"<<endl;
            return 1;
        }
    }
    gpio->gpioGetValue(SW2_PIN, &value);
    if(value!=bn2){
        bn2=value;
        if(bn2==1) {
            //cout<<"bn2"<<endl;
            return 2;
        }
    }
    gpio->gpioGetValue(SW3_PIN, &value);
    if(value!=bn3){
        bn3=value;
        if(bn3==1) {
            //cout<<"bn3"<<endl;
            return 3;
        }
    }
    gpio->gpioGetValue(SW4_PIN, &value);
    if(value!=bn4){
        bn4=value;
        if(bn4==1) {
            //cout<<"bn4"<<endl;
            return 4;
        }
    }
    return -1;
}
int IOInterface::chooseCarMode(){
    int mode=0;
    setSpeed(0);
    setSteer(0);
    setCamAngle(0);
    printLCD(0,0,"MODE(0):THI CU");
    while(true){
        int key=getLCDkey();
        if(key==1){
            mode=(mode+1)%2;
            if(mode==0) printLCD(0,0,"MODE(0):THI CU");
            if(mode==1) printLCD(0,0,"MODE(1):NGHIEM TUC");
        }
        if(key==4){
            return mode;
        }
    }
}
void IOInterface::ready(){
    printLCD(0,0,"---READY!!!---");
    ss1=0;
    while(true){
        int key=getLCDkey();
        if(key==6){
            break;
        }
    }
}
void IOInterface::printLCD(int x,int y,string msg){
    lcd->LCDClear();
    lcd->LCDSetCursor(x, y);
    lcd->LCDPrintStr(msg.c_str());
}
void IOInterface::clearLCD(){
    lcd->LCDClear();
}
void IOInterface::setSteer(float val){
    double temp=val;
    api_set_STEERING_control(pca9685, temp);
}
void IOInterface::setSpeed(float val){
    double temp=val;
    api_set_FORWARD_control(pca9685, temp);
}
void IOInterface::setCamAngle(float val){
    double temp=val;
    api_set_CAMERA_control(pca9685, temp); 
}
Mat IOInterface::getBGR(){
    color.readFrame(&colorFrame);   
    Mat mat_color = cv::Mat(240, 320, CV_8UC3, (uint8_t*)colorFrame.getData());
    cvtColor(mat_color, mat_color, CV_BGR2RGB);
    return mat_color;
}   
Mat IOInterface::getDepth(){
    depth.readFrame(&depthFrame);
    Mat mat_depth = cv::Mat(240, 320, CV_16UC1, (openni::DepthPixel*)depthFrame.getData());
    return mat_depth;
}
Vec3f IOInterface::getIMUPose(){
    return imu_pose;
}
void IOInterface::getIMUThreadFunc(){
    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
    RTIMU *imu = RTIMU::createIMU(settings);
    // Initialise the imu object
    imu->IMUInit();
    // Set the Fusion coefficient
    imu->setSlerpPower(0.02);
    // Enable the sensors
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);
    while (1){
        if(imu->IMURead()){
            RTIMU_DATA imuData = imu->getIMUData();
            //cout<<imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE<<endl;
            imu_pose[0]=imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE;
            imu_pose[1]=imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE;
            imu_pose[2]=imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE;
        }
    }
}

